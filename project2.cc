/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <map>
#include "string"
#include "lexer.h"

using namespace std;
vector<pair<string, vector<string>>> ruleList;
vector<string> rhs;
vector<string> lhs;
vector<string> terminals;
vector<string> nonTerminals;
string symbols[100];
int usefulSize = 0;
map<string, int> map;
LexicalAnalyzer lexer;
Token t;

void parse_Rule_list();
void parse_Rule();
void parse_Right_hand_side();
void parse_Id_list();

void parse_Rule_list(){
    t = lexer.GetToken();

    if(t.token_type == ID){
        string getLexeme = t.lexeme;
        lexer.UngetToken(t);
        parse_Rule();

        ruleList.push_back(make_pair(getLexeme, rhs));
        rhs.clear();

        t = lexer.GetToken();
        lexer.UngetToken(t);
        if(t.token_type == ID){
            parse_Rule_list();
        }else if(t.token_type == DOUBLEHASH){
            return;
        }else{
            //syntax_error();
        }
    }else{
        //syntax_error();
    }
}

void parse_Rule(){
    t = lexer.GetToken();
    if(t.token_type == ID){
        if(find(lhs.begin(), lhs.end(), t.lexeme) == lhs.end()){
            lhs.push_back(t.lexeme);
        }

        t = lexer.GetToken();
        if(t.token_type == ARROW){
            parse_Right_hand_side();
            t = lexer.GetToken();
            if(t.token_type != HASH){
                //syntax_error();
            }
        }else{
            //syntax_error();
        }
    }else{
        //syntax_error();
    }
}

void parse_Right_hand_side() {
    t = lexer.GetToken();
    if(t.token_type == ID) {
        rhs.push_back(t.lexeme);

        t = lexer.GetToken();
        lexer.UngetToken(t);
        if (t.token_type == ID) {
            parse_Id_list();
        }
    }else if(t.token_type == HASH){
        lexer.UngetToken(t);
    }else{
        //syntax_error();
    }
}

void parse_Id_list() {
    t = lexer.GetToken();
    if(t.token_type == ID){
        rhs.push_back(t.lexeme);

        t = lexer.GetToken();
        lexer.UngetToken(t);
        if(t.token_type == ID){
            parse_Id_list();
        }
    }else {
        //syntax_error();
    }
}


// read grammar
void ReadGrammar()
{
    parse_Rule_list();

    for(auto &i : ruleList){
        if(find(nonTerminals.begin(), nonTerminals.end(), i.first) == nonTerminals.end()){
            nonTerminals.push_back(i.first);
        }

        for(auto &j : i.second){
            if(find(lhs.begin(), lhs.end(), j) == lhs.end()){
                if(find(terminals.begin(), terminals.end(), j) == terminals.end()){
                    terminals.push_back(j);
                }
            }else{
                if(find(nonTerminals.begin(), nonTerminals.end(), j) == nonTerminals.end()){
                    nonTerminals.push_back(j);
                }
            }
        }
    }

    symbols[0] = "#";
    symbols[1] = "$";
    usefulSize += 2;
    for(int i = 0; i < terminals.size(); i++){
        symbols[i+2] = terminals[i];
        usefulSize++;
    }
    for(auto &i : nonTerminals) {
        symbols[usefulSize++] = i;
    }
}

// Task 1
void printTerminalsAndNoneTerminals()
{
    string output;
    for(auto &i : terminals){
        output += i + " ";
    }
    output += " ";
    for(auto &i : nonTerminals) {
        output += i + " ";
    }

    cout << output <<endl;
}

// Task 2
void RemoveUselessSymbols()
{

    cout << "2\n";
}

// Task 3
void CalculateFirstSets()
{
    cout << "3\n";
}

// Task 4
void CalculateFollowSets()
{
    cout << "4\n";
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    cout << "5\n";
}


    
int main (int argc, char* argv[])
{
    int task;
    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);

    
    ReadGrammar();  // Reads the input grammar from standard input
                    // and represent it internally in data structures
                    // ad described in project 2 presentation file

    switch (task) {
        case 1: printTerminalsAndNoneTerminals();
            break;

        case 2: RemoveUselessSymbols();
            break;

        case 3: CalculateFirstSets();
            break;

        case 4: CalculateFollowSets();
            break;

        case 5: CheckIfGrammarHasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}

