//
//  Parser.hpp
//  Compiler
//
//  Created by Andrew on 2017/5/2.
//  Copyright © 2017年 AK Inc. All rights reserved.
//

#ifndef Parser_hpp
#define Parser_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <stack>
#include "Lexer.hpp"


const int MAXCHILDLEN = 4;

typedef enum {StmtK, ExpK} EnodeKind;   // type of node: expression, statement
typedef enum {IfK, ReapeatK, AssignK, ReadK, WriteK} EStmtKind; // type of sub of statment: ...
typedef enum {OpK, ConstK, IdK} EExpKind;   // type of sub of expression: ...
typedef enum {Void, Integer, Boolean} EExpType; // value of expression

struct Node {
    struct Node* pChildNode[MAXCHILDLEN];
    struct Node* pSibling;
    
    EnodeKind NodeKind;
    union { EStmtKind stmt; EExpKind exp; } KNode;
    union { TokenType op; int val; char* name; } Attr;
    
    int lineNum;
    EExpType ExpType;
};

/*
 How we record the syntax tree? - By using stack!!!
*/
class Parser {
private:
    std::vector<TokenRecord> tokens;
    Node* syntaxTree;
    size_t lookAhead;  // pointer to the current token record
    
private:
    Node* declara(); // var or statment
    Node* stmtSequence();
    Node* stateMent();   // expression-clause, selection-clause, iteration-clause
    Node* expStmt(); // express ; | ;
    Node* seleStmt();    // match(if) expression match(then) statement match(end)
    Node* iteraStmt();   // match(repeat) expression match(until) expression
    Node* assignStmt();
    Node* simpleExp();   // additiveExp match(relop) additiveExp | additiveExp
    Node* additiveExp(); // additiveExp match(addop) term
    Node* term();    // term match(mulop) factor
    Node* factor();  // match(() exp match()) | match(ID) | match(NUM)
    Node* writeStmt();
    Node* readStmt();

private:
    Node* createStmtNode(EStmtKind);
    Node* createExpNode(EExpKind);
    void report(const std::string, const size_t);
    void match(const TokenType&);
    void printTree(Node*, int);
    void printOp(const TokenType&);

public:
    Parser(std::vector<TokenRecord> tokens): tokens(tokens) { syntaxTree = new Node(); };
    void parser();   // Parsing with TokenRecords
    void show();    // list all SyntaxTree
    void printToken(const size_t);
};

#endif /* Parser_hpp */
