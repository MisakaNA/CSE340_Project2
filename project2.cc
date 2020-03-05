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
bool hasUseless = true;
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
    for(auto &i : terminals){
        symbols[symbolSize++] = i;
        //generateSymbols[symbolSize++] = true;
    }
    for(auto &i : nonTerminals) {
        symbols[symbolSize++] = i;
        //generateSymbols[symbolSize++] = false;
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

//bool gen = false;
/*void isGenerate(bool *generate){
    bool isChanged;
    do {
        isChanged = false;
        for (auto &i : ruleList) {
            //go through rule body vector

            for (auto &j : i.second) {
                //check is there any element not true in usefulSymbol
                int index = distance(symbols, find(symbols, symbols + symbolSize, j));
                if (generate[index]) {
                    gen = true;
                //}else if(){
                } else {
                    //one ungenerating element means whole rule ungenerating
                    gen = false;
                    break;
                }
            }

            //check if all generating or empty (empty means there is only one epsilon) and sign to true in usefulSymbol
            if (i.second.empty() || gen) {
                int index = distance(symbols, find(symbols, symbols + symbolSize, i.first));
                if(!generate[index]){
                    generate[index] = true;
                    isChanged = true;
                }
            }
        }
    }while(isChanged);
}


void isReachable(bool *reachable, vector<pair<string, vector<string>>> ruleGen) {
    bool isChanged = true;
    while(isChanged){
        isChanged = false;
        for (auto &i : ruleGen) {
            int index = distance(symbols, find(symbols, symbols + symbolSize, i.first));
            if (reachable[index]) {
                for (auto &j : i.second) {
                    int tempIdx = distance(symbols, find(symbols, symbols + symbolSize, j));
                    if(!reachable[tempIdx]){
                        reachable[tempIdx] = true;
                        isChanged = true;
                    }
                }
            }
        }
    }
}*/

vector<pair<string, vector<string>>> ruleGen;
vector<pair<string, vector<string>>> useful;
//bool rea = true;
void getUseless(){
    /*bool generateSymbols[symbolSize];
    bool reachableSymbols[symbolSize];
    for (int i = 0; i < symbolSize; i++) {
        generateSymbols[i] = find(terminals.begin(), terminals.end(), symbols[i]) != terminals.end();
    }
    generateSymbols[0] = true;

    //get generate array
    isGenerate(generateSymbols);
    for(auto &i : ruleList){
        if(!i.second.empty()) {
            for (auto &j : i.second) {
                int idx = distance(symbols, find(symbols, symbols + symbolSize, j));
                if (generateSymbols[idx]) {
                    gen = true;
                }else{
                    //one ungenerating element means whole rule ungenerating
                    gen = false;
                    break;
                }
            }
        }else{
            gen = true;
        }

        if(gen){
            ruleGen.emplace_back(i.first, i.second);
        }else{
            //if the rule is not generating it will make the rule list unpridictive
            hasUseless = false;
        }
    }

    if(!ruleGen.empty() && ruleGen[0].first == ruleList[0].first){
        int index = distance(symbols, find(symbols, symbols + symbolSize, ruleList[0].first));
        for(int i = 0; i < symbolSize; i++){
            reachableSymbols[i] = i == index;
        }
        //get reachable array
        isReachable(reachableSymbols, ruleGen);
        for(auto &i : ruleGen){
            for(auto &j : i.second){
                int idx = distance(symbols, find(symbols, symbols + symbolSize, i.first));
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
                hasUseless = false;
            }
        }
    }*/
    //vector<bool> generateArr(symbolSize, false);
    //vector<bool> reachableArr(symbolSize, false);
    bool generateArr[symbolSize];
    bool reachableArr[symbolSize];
    bool isChanged = true;

    for (int i = 0; i < symbolSize; i++) {
        generateArr[i] = find(terminals.begin(), terminals.end(), symbols[i]) != terminals.end();
    }
    generateArr[0] = true;

    while(isChanged){
        isChanged = false;
        for(auto &i : ruleList){
            bool gen = true;
            for(auto &j : i.second){
                int index = distance(symbols, find(symbols, symbols + symbolSize, j));
                if(!generateArr[index]) {
                    gen = false;
                    break;
                }
            }

            if(i.second.empty() || gen){
                int index = distance(symbols, find(symbols, symbols + symbolSize, i.first));
                if(!generateArr[index]){
                    generateArr[index] = true;
                    isChanged = true;
                }
            }
        }
    }

    for(auto &i : ruleList){
        for (auto &j : i.second) {
            int index = distance(symbols, find(symbols, symbols + symbolSize, j));
            if(!i.second.empty()){
                if (generateArr[index]) {
                    ruleGen.emplace_back(i.first, i.second);
                }else{
                    //one ungenerating element means whole rule ungenerating
                    hasUseless = false;
                    break;
                }
            } else {
                ruleGen.emplace_back(i.first, i.second);
            }
        }
    }

    if(!ruleGen.empty() && ruleGen[0].first == ruleList[0].first){
        int index = distance(symbols, find(symbols, symbols + symbolSize, ruleGen[0].first));
        for(int i = 0; i < symbolSize; i++){
            reachableArr[i] = i == index;
        }

        isChanged = true;
        while(isChanged){
            isChanged = false;
            for (auto &i : ruleList) {
                int index2 = distance(symbols, find(symbols, symbols + symbolSize, i.first));
                if (reachableArr[index2]) {
                    for (auto &j : i.second) {
                        int tempIdx = distance(symbols, find(symbols, symbols + symbolSize, j));
                        if(!reachableArr[tempIdx]){
                            reachableArr[tempIdx] = true;
                            isChanged = true;
                        }
                    }
                }
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
    bool epsilon;
    string ruleName;
    vector<string> ruleBody;
    //rule 1
    followSet[nonTerminals[0]].push_back("$");
    //applying rule 4 and 5 to all rules once
    for (auto & i : ruleList) {
        ruleName = i.first;
        ruleBody = i.second;
        for (int j = 0; j < ruleBody.size(); j++) {
            if (find(nonTerminals.begin(), nonTerminals.end(), ruleBody[j]) != nonTerminals.end()) {
                for(int k = j + 1; k < ruleBody.size(); k++){
                    epsilon = false;
                    if (find(firstSet[ruleBody[k]].begin(), firstSet[ruleBody[k]].end(), "#") !=
                        firstSet[ruleBody[k]].end()) {
                        epsilon = true;
                    }
                    for (auto &l : firstSet[ruleBody[k]]) {
                        if (find(followSet[ruleBody[j]].begin(), followSet[ruleBody[j]].end(), l) ==
                            followSet[ruleBody[j]].end()) {
                            if (l != "#") {
                                followSet[ruleBody[j]].push_back(l);
                            }
                        }
                    }
                    if (!epsilon) {
                        break;
                    }
                }
            }
        }
    }
    //applying rule 2 and 3 by loop
    do{
        isChanged = false;
        for (auto & i : ruleList) {
            ruleName = i.first;
            ruleBody = i.second;
            for(int j = 0; j < ruleBody.size(); j++) {
                epsilon = true;
                if (find(nonTerminals.begin(), nonTerminals.end(), ruleBody[j]) != nonTerminals.end()) {
                    if(j != ruleBody.size() - 1){
                        for(int k = j + 1; k < ruleBody.size(); k++) {
                            if(find(firstSet[ruleBody[k]].begin(), firstSet[ruleBody[k]].end(), "#") == firstSet[ruleBody[k]].end()){
                                epsilon = false;
                                break;
                            }
                        }
                        if(epsilon){
                            for(auto &k : followSet[i.first]){
                                if(find(followSet[ruleBody[j]].begin(), followSet[ruleBody[j]].end(), k) == followSet[ruleBody[j]].end()){
                                    followSet[ruleBody[j]].push_back(k);
                                    isChanged = true;
                                }
                            }
                        }
                    }else{
                        for(auto &k : followSet[i.first]){
                            if(find(followSet[ruleBody[j]].begin(), followSet[ruleBody[j]].end(), k) == followSet[ruleBody[j]].end()){
                                followSet[ruleBody[j]].push_back(k);
                                isChanged = true;
                            }
                        }
                    }
                }
            }
        }
    } while(isChanged);
}

// Task 4
void CalculateFollowSets()
{
    getFirst();
    //add first set of terminals and epsilon to firstSet for convenient
    for(auto &i : terminals){
        firstSet[i].push_back(i);
    }
    firstSet["#"].push_back("#");

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

vector<string> rhsFirst(vector<string> ruleBody) {
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
    //update global hasUseless value by calling getUseless
    getUseless();
    if(!hasUseless){
        output = "NO";
    }else{
        //check condition 1 first
        getFirst();
        bool notSatCond1 = true;
        bool breakLoop = false;
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
                    vector<string> first_i = rhsFirst(ruleList[i].second);
                    vector<string> first_j = rhsFirst(ruleList[j].second);
                    //check intersection
                    notSatCond1 = checkIntersection(first_i, first_j);
                    if(notSatCond1){
                        breakLoop = true;
                        break;
                    }
                }
            }
            if(breakLoop){
                break;
            }
        }

        bool notSatCond2 = false;
        if(notSatCond1){
            output = "NO";
        }else{
            getFollow();
            for(auto &i : nonTerminals){
                if(find(firstSet[i].begin(), firstSet[i].end(), "#") != firstSet[i].end()){
                    notSatCond2 = checkIntersection(firstSet[i], followSet[i]);
                }
                if(notSatCond2){
                    break;
                }
            }
            if(notSatCond2){
                output = "NO";
            }else{
                output = "YES";
            }
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

