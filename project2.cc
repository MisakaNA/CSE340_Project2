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
int symbolSize = 0;
bool predParser = true;
map<string, int> map;
LexicalAnalyzer lexer;
Token t;

void parse_Rule_list();
void parse_Rule();
void parse_Right_hand_side();
void parse_Id_list();

void syntax_error()
{
    cout << "SYNTAX ERROR !!!\n";
    exit(1);
}

void parse_Rule_list(){
    t = lexer.GetToken();
    if(t.token_type == ID){
        string getLexeme = t.lexeme;
        lexer.UngetToken(t);
        parse_Rule();

        ruleList.emplace_back(getLexeme, rhs);
        rhs.clear();

        t = lexer.GetToken();
        lexer.UngetToken(t);
        if(t.token_type == ID){
            parse_Rule_list();
        }else if(t.token_type == DOUBLEHASH){
            return;
        }else{
            syntax_error();
        }
    }else{
        syntax_error();
    }
}

void parse_Rule(){
    t = lexer.GetToken();
    if(t.token_type == ID){
        //if(find(lhs.begin(), lhs.end(), t.lexeme) == lhs.end()){
        lhs.push_back(t.lexeme);
        //}

        t = lexer.GetToken();
        if(t.token_type == ARROW){
            parse_Right_hand_side();
            t = lexer.GetToken();
            if(t.token_type != HASH){
                syntax_error();
            }
        }else{
            syntax_error();
        }
    }else{
        syntax_error();
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
        syntax_error();
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
        syntax_error();
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
    symbolSize += 2;
    for(const auto & terminal : terminals){
        symbols[symbolSize++] = terminal;
    }
    for(auto &i : nonTerminals) {
        symbols[symbolSize++] = i;
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


bool checkEqual(const bool arr1[], const bool arr2[]){
    for(int i = 0; i < symbolSize; i++){
        if(arr1[i] != arr2[i]){
            return false;
        }
    }
    return true;
}

bool gen = false;
void isGenerate(bool *useless){
    //save current one
    bool prev[symbolSize];
    for(int i = 0; i < symbolSize; i++){
        prev[i] = useless[i];
    }

    //setGenerate(useless);

    for(auto &i : ruleList){
        //go through rule body vector
        for(auto &j : i.second){
            //check is there any element not true in usefulSymbol
            int index = distance(symbols, find(symbols, symbols + symbolSize, j));
            if(useless[index]){
                gen = true;
            }else{
                //one ungenerating element means whole rule ungenerating
                gen = false;
                break;
            }
        }

        //check if all generating or empty (empty means there is only one epsilon) and sign to true in usefulSymbol
        if(i.second.empty()|| gen){
            int index = distance(symbols, find(symbols, symbols + symbolSize, i.first));
            useless[index] = true;
        }
    }

    if(!checkEqual(prev, useless)){
        isGenerate(useless);
    }
}

// Task 2
vector<pair<string, vector<string>>> ruleGen;
void RemoveUselessSymbols() {
    bool uselessSymbols[symbolSize];
    for (int i = 0; i < symbolSize; i++) {
        uselessSymbols[i] = find(terminals.begin(), terminals.end(), symbols[i]) != terminals.end();
    }
    uselessSymbols[0] = true;
    /*
    bool prev[symbolSize];
    for(int i = 0; i < symbolSize; i++){
        prev[i] = uselessSymbols[i];
    }

    setGenerate(uselessSymbols);

    if(checkEqual(prev, uselessSymbols) == false){
        RemoveUselessSymbols();
    }
     */
    isGenerate(uselessSymbols);

    for(auto &i : ruleList){
        for(auto &j : i.second){
            int idx = distance(symbols, find(symbols, symbols + symbolSize, j));
            if(uselessSymbols[idx]){
                gen = true;
            }else{
                //one ungenerating element means whole rule ungenerating
                gen = false;
                break;
            }
        }

        if(gen){
            ruleGen.emplace_back(i.first, i.second);
        }else{

        }
    }

    if(!ruleGen.empty()){

    }

    cout << "";
}

vector<pair<string, vector<string>>> firstSet;
vector<string> holder;
string firstHolder;
vector<string> secondHolder;
bool hasEpsilon = false;
bool addEpsilon = false;
void getFirst(){
    vector<pair<string, vector<string>>> firstPrev;
    firstPrev.reserve(firstSet.size());
    for(auto &i : firstSet){
        firstPrev.emplace_back(i.first, i.second);
    }


    for(auto &i : nonTerminals) {
        //empty means has only one element and it's epsilon"#"

        for(auto &m : ruleList){
            if(m.first == i && m.second != secondHolder){
                firstHolder = m.first;
                secondHolder = m.second;

                for(auto &j : firstSet){
                    if(find(holder.begin(), holder.end(), j.first) == holder.end()){
                        holder.push_back(j.first);
                    }
                }

                if(secondHolder.empty()){
                    if(find(holder.begin(), holder.end(), firstHolder) == holder.end()){
                        vector<string> set;
                        set.emplace_back("#");
                        firstSet.emplace_back(firstHolder, set);
                    }else{
                        for(auto &j : firstSet){
                            if(j.first == firstHolder){
                                if(find(j.second.begin(), j.second.end(), "#") == j.second.end()){
                                    j.second.insert(j.second.begin(), "#");
                                }
                            }
                        }
                    }
                }
                    //check if the firstSet one is a terminal, add to firstSet if it is.
                else if(find(terminals.begin(), terminals.end(), secondHolder[0]) != terminals.end()){
                    if(find(holder.begin(), holder.end(), firstHolder) == holder.end()){
                        vector<string> set;
                        set.push_back(secondHolder[0]);
                        firstSet.emplace_back(firstHolder, set);
                    }else{
                        for(auto &j : firstSet){
                            if(j.first == firstHolder){
                                if(find(j.second.begin(), j.second.end(), secondHolder[0]) == j.second.end()){
                                    j.second.push_back(secondHolder[0]);
                                }
                            }
                        }
                    }
                }else{
                    //inside here, this symbol must be a non-terminal
                    vector<string> set;
                    for(auto &j : secondHolder){
                        if(find(nonTerminals.begin(), nonTerminals.end(), j) != nonTerminals.end()){
                            if(find(holder.begin(), holder.end(), j) != holder.end()){
                                for(auto &k : firstSet){
                                    if(k.first == j){
                                        if(!k.second.empty()){
                                            for(auto &l : k.second){
                                                if(l != "#"){
                                                    set.push_back(l);
                                                }else{
                                                    hasEpsilon = true;
                                                    addEpsilon = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }else{
                            set.push_back(j);
                            hasEpsilon = false;
                        }

                        if(!hasEpsilon){
                            break;
                        }
                    }

                    if(addEpsilon){
                        set.insert(set.begin(), "#");
                    }

                    //check if it is already there
                    if(find(holder.begin(), holder.end(), firstHolder) == holder.end()){
                        firstSet.emplace_back(firstHolder, set);
                    }else{
                        for(auto &j : firstSet){
                            if(j.first == firstHolder){
                                for(auto &k : set){
                                    if(find(j.second.begin(), j.second.end(), k) == j.second.end()){
                                        j.second.push_back(k);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(firstPrev != firstSet){
        getFirst();
    }
}

// Task 3
void CalculateFirstSets()
{
    getFirst();
    string output;

    vector<string> tempTerminals = terminals;
    tempTerminals.insert(tempTerminals.begin(), "#");

    for(auto &i : firstSet){
        int trackComma = 0;
        output += "FIRST(" + i.first + ") = { ";
        for(auto &j : tempTerminals) {
            if(find(i.second.begin(), i.second.end(), j) != i.second.end()) {
                trackComma++;
                if(trackComma != i.second.size()){
                    output += j + ", ";
                }else{
                    output += j;
                }

            }
        }
        output += " }\n";
    }
    cout << output << endl;
}

// Task 4
void CalculateFollowSets()
{
    cout << "4\n";
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    if(predParser){
        cout << "YES";
    }else{
        cout << "NO";
    }
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
       and the firstSet argument to your program is stored in argv[1]
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

