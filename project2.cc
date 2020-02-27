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
        string getRuleName = t.lexeme;
        lexer.UngetToken(t);
        parse_Rule();

        ruleList.emplace_back(getRuleName, rhs);
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
            if(find(lhs.begin(), lhs.end(), j) == lhs.end()) {
                if (find(terminals.begin(), terminals.end(), j) == terminals.end()) {
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
void printTerminalsAndNoneTerminals() {
    string output;
    for (auto &i : terminals) {
        output += i + " ";
    }
    output += " ";
    for (auto &i : nonTerminals) {
        output += i + " ";
    }

    cout << output << endl;
}

bool gen = false;
void isGenerate(bool *useless){
    bool isChanged = false;
    do {
        isChanged = false;
        for (auto &i : ruleList) {
            //go through rule body vector
            for (auto &j : i.second) {
                //check is there any element not true in usefulSymbol
                int index = distance(symbols, find(symbols, symbols + symbolSize, j));
                if (useless[index]) {
                    gen = true;
                } else {
                    //one ungenerating element means whole rule ungenerating
                    gen = false;
                    break;
                }
            }

            //check if all generating or empty (empty means there is only one epsilon) and sign to true in usefulSymbol
            if (i.second.empty() || gen) {
                int index = distance(symbols, find(symbols, symbols + symbolSize, i.first));
                if(!useless[index]){
                    useless[index] = true;
                    isChanged = true;
                }
            }
        }
    }while(isChanged);

}

bool rea = true;
void isReachable(bool *reachable, vector<pair<string, vector<string>>> ruleGen) {
    bool isChanged = false;
    do{
        isChanged = false;
        for (auto &i : ruleGen) {
            int index = distance(symbols, find(symbols, symbols + symbolSize, i.first));
            if (reachable[index]) {
                for (auto &j : i.second) {
                    //if (find(nonTerminals.begin(), nonTerminals.end(), j) != nonTerminals.end()) {
                        int tempIdx = distance(symbols, find(symbols, symbols + symbolSize, j));
                        if(!reachable[tempIdx]){
                            reachable[tempIdx] = true;
                            isChanged = true;
                        }
                }
            }
        }

    }while(isChanged);
}

vector<pair<string, vector<string>>> ruleGen;
vector<pair<string, vector<string>>> useful;
void getUseless(){
    bool generateSymbols[symbolSize];
    bool reachableSymbols[symbolSize];
    for (int i = 0; i < symbolSize; i++) {
        generateSymbols[i] = find(terminals.begin(), terminals.end(), symbols[i]) != terminals.end();
    }
    generateSymbols[0] = true;

    //get generate array
    isGenerate(generateSymbols);
    for(auto &i : ruleList){
        for(auto &j : i.second){
            int idx = distance(symbols, find(symbols, symbols + symbolSize, j));
            if(generateSymbols[idx]){
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
            //if the rule is not generating it will make the rule list unpridictive
            predParser = false;
        }
    }

    if(!ruleGen.empty()){
        int index = distance(symbols, find(symbols, symbols + symbolSize, ruleList[0].first));
        for(int i = 0; i < symbolSize; i++){
            reachableSymbols[i] = i == index;
        }

        //get reachable array
        isReachable(reachableSymbols, ruleGen);
        for(auto &i : ruleGen){
            for(auto &j : i.second){
                int idx = distance(symbols, find(symbols, symbols + symbolSize, j));
                if(reachableSymbols[idx]){
                    rea = true;
                }else{
                    //one ungenerating element means whole rule ungenerating
                    rea = false;
                    break;
                }
            }

            if(rea){
                //all reachable rules from ruleGen is useful
                useful.emplace_back(i.first, i.second);
            }else{
                //if the rule is unreachable it will make the rule list unpridictive
                predParser = false;
            }
        }
    }
}

// Task 2
void RemoveUselessSymbols() {
    //update useful
    getUseless();
    if(!useful.empty()){
        string output;
        for(auto &i : useful){
            output += i.first + " -> ";
            if(!i.second.empty()){
                for(auto &j : i.second){
                    output += j + " ";
                }
            }else{
                output += "#";
            }
            output += "\n";
        }
        cout << output;

    }else{
        cout << "";
    }
}


map<string, vector<string>> firstSet;

void getFirst(){
    map<string, vector<string>> firstPrev;
    bool hasEpsilon = false;
    string ruleName;
    vector<string> ruleBody;
    bool isChanged;
    do{
        isChanged = false;
        for(auto &i : ruleList){
            ruleName = i.first;
            ruleBody = i.second;
            //empty means #
            if(ruleBody.empty()){
                if(find(firstSet[ruleName].begin(), firstSet[ruleName].end(), "#") == firstSet[ruleName].end()){
                    firstSet[ruleName].push_back("#");
                    isChanged = true;
                }
            }else if(find(terminals.begin(), terminals.end(), ruleBody[0]) != terminals.end()){
                if(find(firstSet[ruleName].begin(), firstSet[ruleName].end(), ruleBody[0]) == firstSet[ruleName].end()){
                    firstSet[ruleName].push_back(ruleBody[0]);
                    isChanged = true;
                }
            }else{
                for(auto &j : ruleBody){
                    hasEpsilon = false;
                    if(find(nonTerminals.begin(), nonTerminals.end(), j) != nonTerminals.end()){
                        vector<string> tempFirst;
                        for(auto &k : firstSet[j]){
                            if(k != "#"){
                                if(find(firstSet[ruleName].begin(), firstSet[ruleName].end(), k) == firstSet[ruleName].end()) {
                                    firstSet[ruleName].push_back(k);
                                    isChanged = true;
                                }
                            }else{
                                hasEpsilon = true;
                            }
                        }
                        if(!hasEpsilon){
                            break;
                        }
                    } else{
                        if(find(firstSet[ruleName].begin(), firstSet[ruleName].end(), j) == firstSet[ruleName].end()) {
                            firstSet[ruleName].push_back(j);
                            isChanged = true;
                        }
                        break;
                    }
                }
                if(hasEpsilon){
                    if (find(firstSet[ruleName].begin(), firstSet[ruleName].end(), "#") == firstSet[ruleName].end()) {
                        firstSet[ruleName].insert(firstSet[ruleName].begin(), "#");
                        isChanged = true;
                    }
                }
            }
        }
    }while(isChanged);
}

// Task 3
void CalculateFirstSets()
{
    getFirst();
    string output;

    vector<string> tempTerminals = terminals;
    tempTerminals.insert(tempTerminals.begin(), "#");

    for(auto &i : nonTerminals){
        int trackComma = 0;
        output += "FIRST(" + i + ") = { ";
        for(auto &j : tempTerminals){
            if(find(firstSet[i].begin(), firstSet[i].end(), j) != firstSet[i].end()){
                trackComma++;
                if(trackComma != firstSet[i].size()){
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

map<string, vector<string>> followSet;
void getFollow(){
    bool isChanged;
    bool epsilon = false;
    string ruleName;
    vector<string> ruleBody;
    //rule 1
    followSet[nonTerminals[0]].push_back("$");
    do{
        isChanged = false;
        for (auto &m : ruleList) {
            ruleName = m.first;
            ruleBody = m.second;
            //loop body
            for(int i = 0; i < ruleBody.size(); i++){
                if(find(nonTerminals.begin(), nonTerminals.end(), ruleBody[i]) != nonTerminals.end()){
                    //get the following body after first element
                    vector<string> followVec = vector<string> (ruleBody.begin() + i + 1, ruleBody.end());
                    //if end of line
                    if(followVec.empty()){
                        //rule 2 A-> xxx B
                        for(auto &j : followSet[ruleName]){
                            if(find(followSet[ruleBody[i]].begin(), followSet[ruleBody[i]].end(), j) == followSet[ruleBody[i]].end()){
                                followSet[ruleBody[i]].push_back(j);
                                isChanged = true;
                            }
                        }
                    }else{
                        //regular possibilities
                        for(auto &j : followVec){
                            epsilon = false;
                            // A -> B a;
                            if(find(terminals.begin(), terminals.end(), j) != terminals.end()){
                                if(find(followSet[ruleBody[i]].begin(), followSet[ruleBody[i]].end(), j) == followSet[ruleBody[i]].end()){
                                    followSet[ruleBody[i]].push_back(j);
                                    isChanged = true;
                                }
                                break;
                            }else{
                                //rule 4 and 5  A -> B C D
                                for(auto &l : firstSet[j]){
                                    if(l != "#"){
                                        if(find(followSet[ruleBody[i]].begin(), followSet[ruleBody[i]].end(), l) == followSet[ruleBody[i]].end()){
                                            followSet[ruleBody[i]].push_back(l);
                                            isChanged = true;
                                        }
                                    }else{
                                        epsilon = true;
                                    }
                                }
                                if(!epsilon){
                                    break;
                                }
                            }
                        }
                    }
                    //rule 3 A -> xxxxx B a b c d e    epsilon belongs to abcde
                    if(epsilon){
                        // A -> B a b c d e    epsilon belongs to abcde
                        if(find(nonTerminals.begin(), nonTerminals.end(), ruleBody[0]) != nonTerminals.end()){
                            for(auto &j : followSet[ruleName]){
                                if(find(followSet[ruleBody[0]].begin(), followSet[ruleBody[0]].end(), j) == followSet[ruleBody[0]].end()){
                                    followSet[ruleBody[0]].push_back(j);
                                    isChanged = true;
                                }
                            }
                        }else{
                            //A -> xxxxx B a b c d e    epsilon belongs to abcde
                            string temp;
                            for(auto &j : ruleBody){
                                if(find(nonTerminals.begin(), nonTerminals.end(), j) != nonTerminals.end()){
                                    temp = j;
                                    break;
                                }
                            }
                            for(auto &j : followSet[ruleName]){
                                if(find(followSet[temp].begin(), followSet[temp].end(), j) == followSet[temp].end()){
                                    followSet[temp].push_back(j);
                                    isChanged = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }while(isChanged);
}

// Task 4
void CalculateFollowSets()
{
    getFirst();
    getFollow();
    string output;
    vector<string> tempTerminal = terminals;
    tempTerminal.insert(tempTerminal.begin(), "$");
    for(auto &i : nonTerminals){
        int commaTracker = 0;
        output += "FOLLOW(" + i + ") = { ";
        for(auto &j : tempTerminal){
            if(find(followSet[i].begin(), followSet[i].end(), j) != followSet[i].end()){
                commaTracker++;
                if(commaTracker != followSet[i].size()){
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

vector<string> ruleFirstSet(vector<string> ruleBody) {
    vector<string> result;
    bool hasEpsilon = true;
    if (ruleBody.empty()) {
        result.emplace_back("#");
    }else{
        for(auto &i : ruleBody){
            hasEpsilon = true;
            if(find(firstSet[i].begin(), firstSet[i].end(), "#") == firstSet[i].end()){
                hasEpsilon = false;
            }
            for(auto &j : firstSet[i]){
                if(find(result.begin(), result.end(), j) == result.end()){
                    if(j != "#"){
                        result.push_back(j);
                    }
                }
            }
            if(!hasEpsilon){
                break;
            }
        }
        if(hasEpsilon){
            result.emplace_back("#");
        }
    }
    return result;
}

bool checkIntersection(vector<string> vec1, vector<string> vec2){
    for(auto &i : vec1){
        if(find(vec2.begin(), vec2.end(), i) != vec2.end()){
            return true;
        }
    }
    return false;
}
// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    string output;
    //update global predParser value by calling getUseless
    getUseless();
    if(!predParser){
        output = "NO";
    }else{
        //check condition 1 first
        getFirst();
        bool satisfiedCond1 = true;
        //add first set of terminals and epsilon to firstSet for convenient
        for(auto &i : terminals){
            firstSet[i].push_back(i);
        }
        firstSet["#"].push_back("#");


        for(int i = 0; i < ruleList.size(); i++){
            //get followed rules
            for(int j = i + 1; j < ruleList.size(); j++){
                //find similar declared rules
                if(ruleList[i].first == ruleList[j].first){
                    vector<string> result_i = ruleFirstSet(ruleList[i].second);
                    vector<string> result_j = ruleFirstSet(ruleList[j].second);
                    //check intersection
                    satisfiedCond1 = checkIntersection(result_i, result_j);
                }
                if(satisfiedCond1){
                    break;
                }
            }
            if(satisfiedCond1){
                break;
            }
        }
        if(satisfiedCond1){
            output = "NO";
        }else{
            getFollow();

        }

    }
    cout << output;
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

