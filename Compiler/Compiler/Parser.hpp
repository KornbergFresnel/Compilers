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

/*
 1. program -> declaration_list
 2. declaration_list -> declaration_list declaration  |  declaration
 3. declaration -> var_declaration  |  statement_list
 4. var_declaration -> write ID | read ID
 5. statement_list -> statement_list statement  |  empty
 6. statement -> expression_stmt  |  selection_stmt  |  iteration_stmt
 7. expression_stmt -> expression ;  |  ;
 8. selection_stmt -> if expression then statement end;
 9. iteration_stmt -> repeat statement until expression
 10. expression -> var := expression  |  simple_expression
 11. var -> ID
 12. simple_expression -> additive_expression relop additive_expression  |  additive_expression
 13. relop -> <  |  >  |  =
 14. additive_expression -> additive_expression addop term  |  term
 15. addop -> +  |  -
 16. term -> term mulop factor  |  factor
 17. mulop -> *  |  /
 18. factor -> ( expression )  |  var  |  NUM
 */
const int MAXCHILDLEN = 255;

typedef enum {StmtK, ExpK} EnodeKind;   // type of node: expression, statement
typedef enum {IfK, ReapeatK, AssignK, ReadK, WriteK} EStmtKind; // type of sub of statment: ...
typedef enum {OpK, ConstK, IdK} EExpKind;   // type of sub of expression: ...
typedef enum {Void, Integer, Boolean} EExpType; // value of expression

typedef struct Node {
    TokenRecord token;  // current node's value, type: TokenRecord
    struct Node* pChildNode[MAXCHILDLEN];
    struct Node* pSibling;
    
    EnodeKind NodeKind;
    union { EStmtKind stmt; EExpKind exp; } Kind;
    union { TokenType op; int val; char* name; } Attr;
    
    int lineNum;
    EExpType ExpType;
} SyntaxTree;

/*
 How we record the syntax tree? - By using stack!!!
*/
class Parser {
private:
    std::vector<TokenRecord> tokens;
    std::vector<SyntaxTree> tree;
    std::vector<SyntaxTree>::iterator iter; // ptr of Lexer result
    size_t lookAhead;  // pointer to the current token record
    
private:
    void match(const TokenType&);
    void declara(); // var or statment
    void stateMent();   // expression-clause, selection-clause, iteration-clause
    void varDeclara();  // match(write) ID | match(read) ID
    void expStmt(); // express ; | ;
    void seleStmt();    // match(if) expression match(then) statement match(end)
    void iteraStmt();   // match(repeat) expression match(until) expression
    void exp(); // ID match(:=) expression | simplepression
    void simpleExp();   // additiveExp match(relop) additiveExp | additiveExp
    void additiveExp(); // additiveExp match(addop) term
    void term();    // term match(mulop) factor
    void factor();  // match(() exp match()) | match(ID) | match(NUM)
    void relop();   // < | > | =
    void addop();   // + | -
    void mulop();   // * | /
    void report(const std::string);

public:
    Parser(std::vector<TokenRecord> tokens): tokens(tokens) {};
    void parser();   // Parsing with TokenRecords
    void show();    // list all SyntaxTree
};

#endif /* Parser_hpp */
