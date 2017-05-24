//
//  Lexer.hpp
//  Compiler
//
//  Created by Andrew on 2017/4/18.
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
    S_START, S_NAME, S_NUM, S_OPE, S_COMMENT, S_END
};

// define token type for lexer
enum TokenType {
    // book-keeping tokens: 0-1
    ENDFILE, ERROR,
    // reserved words: 2-9
    IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,
    // multi character tokens: 10-11
    ID, NUM,
    // special symbols: 12-24
    ASSIGN, EQ, LT, LQ, RT, RQ, PLUS, MINUS, TIMES, OVER, LPAREN, RPAREN, SEMI,
    RESERVED
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
    int bufferPtr;  // ptr of buffer
    int curline;    // this variable records the current line number which Lexer is scanning
    char buffer[MAXN];  // a buffer array used to store scaned chars temporary
    
    std::string curStr;
    std::vector<TokenRecord> eleRecordList;
    // std::vector<int> lineNumber;
    
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
    StateType inOperator(StateType&, char); // 2nd layer case: execute S_OPE
    StateType inComment(StateType&, char);  // 2nd layer case: execute S_COMMENT

public:
    Lexer(std::string sourceFileName): fileName(sourceFileName){}
    std::vector<TokenRecord> getTokenList();
    void lexer();   // open source file, then execute lexer
    void show();    // print lexer result on console
    void store();   // store eleRecordList in a file
};

#endif /* Lexer_hpp */
