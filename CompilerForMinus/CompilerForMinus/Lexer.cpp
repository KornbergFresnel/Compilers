//
//  Lexer.cpp
//  CompilerForMinus
//
//  Created by Andrew on 2017/4/23.
//  Copyright © 2017年 AK Inc. All rights reserved.
//

#include "Lexer.hpp"
#include <ctype.h>
#include <string>
#include <utility>
#include <iostream>

std::map<std::string, TokenType> KEY_WORDS {
    {"if", IF}, {"else", ELSE}, {"int", INT}, {"return", RETURN}, {"void", VOID}, {"while", WHILE},
};

std::map<std::string, TokenType> SPES {
    {"=", ASSIGN}, {"==", EQ}, {"!=", NQ}, {"<", LT}, {"<=", LQ}, {">", RT}, {">=", RQ},
    {"+", PLUS}, {"-", MINUS}, {"*", TIMES}, {"/", OVER}, {"(", LPAREN}, {")", RPAREN},
    {"[", LMP}, {"]", RMP}, {"{", LLP}, {"}", RLP}, {";", SEMI}, {",", COM}
};

/*
 Private functions part: init, getNextChar, getToken
 */
bool Lexer::init() {
    sourceFile.open(fileName);
    if (!sourceFile.is_open()) {
        std::cerr << "Open file fail!" << std::endl;
        return false;
    }
    memset(buffer, 0, sizeof(buffer));
    bufferPtr = -1;
    curline = 0;
    curStr = "";
    return true;
}

char Lexer::getNextChar() {
    return buffer[++bufferPtr];
}

TokenType Lexer::getToken(const std::string& str) {
    if (isdigit(str[0])) {
        return NUM;
    } else if (isalpha(str[0])) {
        // identify keyword or id
        if (KEY_WORDS.find(str) != KEY_WORDS.end()) return KEY_WORDS[str];
        return ID;
    } else {
        return SPES[str];
    }
}

TokenType Lexer::getToken2(const std::string &str) {
    if (isdigit(str[0])) {
        return NUM;
    } else if (isalpha(str[0])) {
        // identify keyword or id
        if (KEY_WORDS.find(str) != KEY_WORDS.end()) return RESERVED;
        return ID;
    } else {
        return SPES[str];
    }
}

void Lexer::storeToken(const std::string& str) {
    TokenRecord tokenRecord;
    tokenRecord.tokenVal = getToken(str);
    if (tokenRecord.tokenVal != NUM) {
        tokenRecord.attribute.stringVal = new char[str.length() + 1];
        std::copy(str.begin(), str.end(), tokenRecord.attribute.stringVal);
    } else {
        tokenRecord.attribute.numVal = std::atoi(str.c_str());
    }
    tokenRecord.lineNum = curline;
    eleRecordList.push_back(tokenRecord);
}

/*
 Private functions part: STATE functions
 */
StateType Lexer::stateSwitch(StateType &state, char w) {
    StateType newState = state;
    switch (state) {
        case S_START:
            newState = inStart(state, w);
            break;
        case S_NAME:
            newState = inName(state, w);
            break;
        case S_SPE:
            newState = inSpecial(state, w);
            break;
        case S_NUM:
            newState = inNum(state, w);
            break;
        case S_COMMENT:
            newState = inComment(state, w);
            break;
        default:
            break;
    }
    return newState;
}

StateType Lexer::inStart(StateType& oldState, char w) {
    
    if (isblank(w)) {
        return S_START;
    } else if (isdigit(w)) {
        curStr += w;
        return S_NUM;
    } else if (isalpha(w)) {
        curStr += w;
        return S_NAME;
    } else if (w == '/') {
        // is over or comment?
        char nextChar = getNextChar();
        if (nextChar == '*') return S_COMMENT;
        else {
            curStr += w;
            return inSpecial(oldState, nextChar);
        }
    }
    std::string temp = "";
    temp += w;
    if (SPES[temp] >= 10 && SPES[temp] <= 28) {
        curStr += w;
        return S_SPE;
    }
    return oldState;
}

StateType Lexer::inComment(StateType &oldState, char w){
    StateType newState;
    if (w == '*') {
        char nextChar = getNextChar();
        if (nextChar == '/') newState = S_START;
        else newState = oldState;
    } else newState = oldState;
    return newState;
}

StateType Lexer::inNum(StateType &oldState, char w) {
    if (isdigit(w)) {
        curStr += w;
        return S_NUM;
    } else {
        // roll back a char without error information
        // store this TokenType
        storeToken(curStr);
        curStr.clear();
        bufferPtr--;
        return S_END;
    }
}

StateType Lexer::inName(StateType &oldState, char w) {
    if (isalpha(w)) {
        curStr += w;
        return S_NAME;
    } else {
        // roll back a char without error information
        // check wheter keyword or id name
        storeToken(curStr);
        curStr.clear();
        bufferPtr--;
        return S_END;
    }
}

StateType Lexer::inSpecial(StateType &oldState, char w) {
    if (w == '=') {
        // accept nex char, legally: next char equals '=' or blank
        curStr += '=';
        storeToken(curStr);
        curStr.clear();
        return S_END;
    } else {
        storeToken(curStr);
        curStr.clear();
        bufferPtr--;
        return S_END;
    }
}

/*
 Public functions part
 */
std::vector<TokenRecord> Lexer::getTokenList() {
    return eleRecordList;
}

void Lexer::lexer() {
    if(init()) {
        char w = '\0';
        StateType state = S_START;
        while (!sourceFile.eof()) {
            if (w == '\0') {
                bufferPtr = -1;
                sourceFile.getline(buffer, PER_SIZE);
                curline++;
            }
            while (state != S_END && (w = getNextChar())) {
                state = stateSwitch(state, w);
            }
            state = (state == S_COMMENT ? S_COMMENT : S_START);
        }
        if (curStr != "") {
            storeToken(curStr);
        }
        TokenRecord token;
        token.tokenVal = ENDFILE;
        std::string eof = "EOF";
        token.attribute.stringVal = new char[4];
        std::copy(eof.begin(), eof.end(), token.attribute.stringVal);
        token.lineNum = curline;
        eleRecordList.push_back(token);
    }
}

void Lexer::show() {
    puts("*****************************************\nC-Minus Lexical Analazer Result:\n*****************************************");
    size_t len = eleRecordList.size();
    for (size_t i = 0; i < len; i++) {
        auto ele = eleRecordList[i];
        printf("%d:\t", ele.lineNum);
        if (ele.tokenVal == NUM) {
            printf("NUM, value = %d\n", ele.attribute.numVal);
        } else if (ele.tokenVal == ID) {
            std::cout << "ID, name = " << ele.attribute.stringVal << std::endl;
        } else if (ele.tokenVal == RESERVED) {
            std::cout << "reserved word: " << ele.attribute.stringVal << std::endl;
        } else {
            std::cout << ele.attribute.stringVal << std::endl;
        }
    }
}

void Lexer::store() {
    return;
}
