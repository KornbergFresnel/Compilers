//
//  Parser.cpp
//  Compiler
//
//  Created by Andrew on 2017/5/2.
//  Copyright © 2017年 AK Inc. All rights reserved.
//

#include "Parser.hpp"

/*
 public function part
 */
void Parser::parser() {
    size_t len = tokens.size();
    while (lookAhead < len) {
        declara();
    }
}

/*
 private function part
 */
void Parser::match(const TokenType& type) {
    // This function is designed for judging wheter token's type equals to judge
    if (tokens[lookAhead].tokenVal == type) lookAhead++;
    else report("syntax error");
}

// Has tow kinds of declarations: var declaration & statement list
void Parser::declara() {
    // judge wheter var declara of statement
    switch (tokens[lookAhead].tokenVal) {
        case WRITE:
        case READ:
            varDeclara();
            break;
        default:
            stateMent();
            break;
    }
}

// Statement contains three kinds of sub-statment: IF, REPEAT and expression
void Parser::stateMent() {
    switch (tokens[lookAhead].tokenVal) {
        case IF:
            seleStmt();
            break;
        case REPEAT:
            iteraStmt();
            break;
        default:
            expStmt();
            break;
    }
}

// Var declara contains only two kinds sub-statement: WRITE or READ
void Parser::varDeclara() {
    switch (tokens[lookAhead].tokenVal) {
        case WRITE:
            match(WRITE); match(ID);
            break;
        case READ:
            match(READ); match(ID);
            break;
        default:
            break;
    }
}

// Expression statment contains only tow kinds expression: expression ; | ;
void Parser::expStmt() {
    switch (tokens[lookAhead].tokenVal) {
        case SEMI:
            match(SEMI);
            break;
        default:
            exp();
            break;
    }
}

// if expression then statement end
void Parser::seleStmt() {
    match(IF); exp(); match(THEN); stateMent(); match(END);
}

// repeat statement until expression
void Parser::iteraStmt() {
    match(REPEAT); stateMent(); match(UNTIL); exp();
}

// Expression contains two kinds: var := expression | simple_expression
void Parser::exp() {
    switch (tokens[lookAhead].tokenVal) {
        case ID:
            match(ID); match(ASSIGN); exp();
            break;
        default:
            simpleExp();
            break;
    }
}

// Simple expression contains two kinds: additiveExp relop additiveExp | additiveExp
// relop: < | > | =
void Parser::simpleExp() {
    additiveExp();
    TokenType tmp = tokens[lookAhead].tokenVal;
    if (tmp == EQ || tmp == LT || tmp == RT) {
        relop();
        additiveExp();
    }
}

void Parser::additiveExp() {
    term();
    TokenType tmp = tokens[lookAhead].tokenVal;
    if (tmp == PLUS || tmp == MINUS) {
        addop();
        additiveExp();
    }
}

void Parser::relop() {
    switch (tokens[lookAhead].tokenVal) {
        case LT:
            match(LT);
            break;
        case RT:
            match(RT);
            break;
        default:
            match(EQ);
            break;
    }
}

void Parser::addop() {
    if (tokens[lookAhead].tokenVal == PLUS) match(PLUS);
    else match(MINUS);
}

void Parser::term() {
    factor();
    TokenType tmp = tokens[lookAhead].tokenVal;
    if (tmp == TIMES || tmp == OVER) {
        mulop();
        term();
    }
}

void Parser::factor() {
    switch (tokens[lookAhead].tokenVal) {
        case LPAREN:
            match(LPAREN); exp(); match(RPAREN);
            break;
        case ID:
            match(ID);
            break;
        default:
            match(NUM);
            break;
    }
}

void Parser::mulop() {
    switch (tokens[lookAhead].tokenVal) {
        case TIMES:
            match(TIMES);
            break;
        default:
            match(OVER);
            break;
    }
}
