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

typedef struct Node {
    TokenRecord token;  // current node's value, type: TokenRecord
    Node* lChild;   // left child node
    Node* rChild;   // right child node
} SyntaxTree;

class Parser {
private:
    std::vector<SyntaxTree> tree;
    std::vector<SyntaxTree>::iterator iter; // ptr of Lexer result
    
private:
    void match(const TokenRecord&, const TokenType&);
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

public:
    Parser() {};
    void parser(const std::vector<TokenRecord>&);
};

#endif /* Parser_hpp */
