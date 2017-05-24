//
//  Lexer.hpp
//  CompilerForMinus
//
//  Created by Andrew on 2017/4/23.
//  Copyright © 2017年 AK Inc. All rights reserved.
//

#ifndef Lexer_hpp
#define Lexer_hpp

#include <stdio.h>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
#include <map>

#define MAXN 256
#define PER_SIZE 100

// define state for DFA
enum StateType {
    S_START, S_NAME, S_NUM, S_SPE, S_COMMENT, S_END
};

// define token type for lexer
enum TokenType {
    // book-keeping tokens: 0-1
    ENDFILE = 0, ERROR = 1,
    // reserved words: 2-7
    IF = 2, ELSE, INT, RETURN, VOID, WHILE = 7,
    // multi character tokens: 8-9
    ID = 8, NUM = 9,
    // operation symbol: 10-21 (=, ==, !=, <, <=, >, >=, +, -, *, /, (, ), [, ]
    ASSIGN = 10, EQ, NQ, LT, LQ, RT, RQ, PLUS, MINUS, TIMES, OVER, LPAREN, RPAREN, LMP, RMP = 24,
    // special symbols: {, }, ;, ,
    LLP = 25, RLP, SEMI, COM = 28,
    RESERVED = 29
};

// Data structure for storing the word's infomation
typedef struct {
    TokenType tokenVal;
    union {
        char* stringVal;
        int numVal;
    } attribute;
    int lineNum;
} TokenRecord;

class Lexer {
private:
    std::ifstream sourceFile;
    std::ofstream outFile;
    std::string fileName;
    int bufferPtr;
    int curline;
    char buffer[MAXN];
    
    std::string curStr;
    std::vector<TokenRecord> eleRecordList;

private:
    bool init();
    void storeToken(const std::string&);
    char getNextChar();
    TokenType getToken(const std::string&);
    TokenType getToken2(const std::string&);
    
    StateType stateSwitch(StateType&, char);
    StateType inStart(StateType&, char);    // 2nd layer case: execute S_START
    StateType inName(StateType&, char);     // 2nd layer case: execute S_NAME
    StateType inNum(StateType&, char);      // 2nd layer case: execute S_NUM
    StateType inSpecial(StateType&, char);  // 2nd layer case: execute S_SPE
    StateType inComment(StateType&, char);  // 2nd layer case: execute S_COMMENT
    
public:
    Lexer(std::string sourceFileName): fileName(sourceFileName){}
    std::vector<TokenRecord> getTokenList();
    void lexer();   // open source file, then execute lexer
    void show();    // print lexer result on console
    void store();   // store eleRecordList in a file
};
#endif /* Lexer_hpp */
