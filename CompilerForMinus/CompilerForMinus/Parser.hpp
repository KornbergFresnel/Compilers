//
//  Parser.hpp
//  CompilerForMinus
//
//  Created by Andrew on 2017/5/24.
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

typedef enum {StmtK, ExpK, DeclaK} EnodeKind;   // type of node: expression, statement
typedef enum {IfK, WhileK, AssignK, CompK, CallK} EStmtKind; // type of sub of statment: ...
typedef enum {Var_DeclK, Arry_ElemK, Funck, Arry_DeclK, ParamsK, VoidK, ParamK} EDeclKind;
typedef enum {OpK, ConstK, IdK, ArrayK, ReturnK} EExpKind;   // type of sub of expression: ...
typedef enum {ParamK, VoidK} EParamKind;
typedef enum {Void, Integer} EExpType; // value of expression

struct Node {
    struct Node* pChildNode[MAXCHILDLEN];
    struct Node* pSibling;
    
    EnodeKind NodeKind; // first check
    union { EStmtKind stmt; EExpKind exp; EDeclKind decla} KNode;   // second check
    union { EParamKind para, TokenType op; int val; char* name; } Attr; // third check
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
    Node* declaraSequence();    // decalration_list decalration | decalration
    Node* declara(); // var or statment
    Node* typeDefine();
    Node* params();
    Node* paramlist();
    Node* param();
    Node* compStmt();
    Node* localDecla();
    Node* stmtSequence();
    Node* stateMent();   // expression-clause, selection-clause, iteration-clause
    Node* expStmt(); // express ; | ;
    Node* seleStmt();    // match(if) expression match(then) statement match(end)
    Node* iteraStmt();   // match(repeat) expression match(until) expression
    Node* returnStmt();
    Node* expression();
    Node* simpleExp();   // additiveExp match(relop) additiveExp | additiveExp
    Node* var();
    Node* term();    // term match(mulop) factor
    Node* factor();  // match(() exp match()) | match(ID) | match(NUM)
    Node* additiveExp();
    Node* call();
    Node* args();
    Node* arglist();
    
private:
    Node* createStmtNode(EStmtKind);
    Node* createExpNode(EExpKind);
    Node* createTypeNode(EExpType);
    Node* createParamNode(EParamKind);
    Node* createDecl();
    bool isRelop(const TokenType);
    void report(const std::string, const size_t);
    void match(const TokenType&);
    void printTree(Node*, int);
    void printOp(const TokenType&);
    void printStmtNode(Node*, int);
    void printExpNode(Node*, int);
    void printDeclaNode(Node*, int);
    
public:
    Parser(std::vector<TokenRecord> tokens): tokens(tokens) { syntaxTree = new Node(); };
    void parser();   // Parsing with TokenRecords
    void show();    // list all SyntaxTree
    void printToken(const size_t);
};
#endif /* Parser_hpp */
