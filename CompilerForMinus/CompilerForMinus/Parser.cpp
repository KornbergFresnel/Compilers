//
//  Parser.cpp
//  CompilerForMinus
//
//  Created by Andrew on 2017/5/24.
//  Copyright © 2017年 AK Inc. All rights reserved.
//

#include "Parser.hpp"
#include <cstring>

/*
 public function part
 */
void Parser::parser() {
    // always push a new node to treeNodeStack when we occurs a new non-terminate node
    // and when we end a syntax of non-terminate node, pop it and push a new node
    lookAhead = 0;
    syntaxTree = declaraSequence();
    if (tokens[lookAhead].tokenVal != ENDFILE)
        report("Code ends before file\n", -1);
}

// Print operation symbols
void Parser::printOp(const TokenType& token) {
    switch (token) {
        case PLUS:
            printf("Op: +\n");
            break;
        case OVER:
            printf("Op: /\n");
            break;
        case MINUS:
            printf("Op: -\n");
            break;
        case TIMES:
            printf("Op: *\n");
            break;
        case LT:
            printf("Op: <\n");
            break;
        case LQ:
            printf("Op: <=\n");
            break;
        case RT:
            printf("Op: >\n");
            break;
        case RQ:
            printf("Op: >=\n");
            break;
        case ASSIGN:
            printf("Op: =\n");
            break;
        case EQ:
            printf("Op: ==\n");
            break;
        case NQ:
            printf("Op: !=\n");
            break;
        default:
            break;
    }
}

// Print tree
void Parser::printTree(Node *node, int layer) {
    if (NULL == node) return;
    for (int i = 0; i < layer; i++) printf("\t");
    if (node->NodeKind == StmtK) {
        switch (node->KNode.stmt) {
            case IfK:
                printf("If\n");
                printTree(node->pChildNode[0], layer + 1);
                printTree(node->pChildNode[1], layer + 1);
                break;
            case WhileK:
                printf("Repeat\n");
                printTree(node->pChildNode[0], layer + 1);
                printTree(node->pChildNode[1], layer + 1);
                break;
            case AssignK:
                printf("Assign to: %s\n", node->Attr.name);
                printTree(node->pChildNode[0], layer + 1);
                break;
            case CompK:
                printf("Read: %s\n", node->Attr.name);
                break;
            case Var_DeclK:
                printf("Write\n");
                printTree(node->pChildNode[0], layer + 1);
                break;
            case FuncK:
                break;
            case ParamsK:
                break;
            case Arry_ElemK:
                break;
            default:
                break;
        }
    } else if (node->NodeKind == ExpK) {
        switch (node->KNode.exp) {
            case OpK:
                printOp(node->Attr.op);
                printTree(node->pChildNode[0], layer + 1);
                printTree(node->pChildNode[1], layer + 1);
                break;
            case ConstK:
                printf("Const: %d\n", node->Attr.val);
                break;
            case IdK:
                printf("Id: %s\n", node->Attr.name);
                break;
            default:
                break;
        }
    }
    printTree(node->pSibling, layer);
}

void Parser::show() {
    printf("******\nSyntax tree:\n*****\n");
    Node* cur = syntaxTree;
    int layer = 0;
    printTree(cur, layer);
}

/*
 private function part
 */
void Parser::match(const TokenType& type) {
    // This function is designed for judging wheter token's type equals to judge
    if (tokens[lookAhead].tokenVal == type) {
        // push to subTreeStack
        lookAhead++;
    }
    else { report("unexcepted token->", lookAhead); lookAhead++; }
}

void Parser::report(const std::string mess, const size_t current) {
    if (current < -1) {
        std::string error_head = ">>> Syntax error at line ";
        std::cout << error_head << tokens[current].lineNum << ": " << mess << " ";
        printToken(current);
    } else std::cerr << mess << std::endl;
}

void Parser::printToken(const size_t current) {
    std::string result;
    if (tokens[current].tokenVal == NUM) {
        printf("NUM, value = %d\n", tokens[current].attribute.numVal);
    } else if (tokens[current].tokenVal == ID) {
        std::cout << "ID, name = " << tokens[current].attribute.stringVal << std::endl;
    } else if (tokens[current].tokenVal == RESERVED) {
        std::cout << "reserved word: " << tokens[current].attribute.stringVal << std::endl;
    } else {
        std::cout << tokens[current].attribute.stringVal << std::endl;
    }
    
}

// declaration_list declaration | declaration
Node* Parser::declaraSequence() {
    Node* t = declara();
    Node* p = t;
    while (tokens[lookAhead].tokenVal != ENDFILE) {
        Node* q;
        match(SEMI);
        q = declara();
        if (q != NULL) {
            if (t == NULL) {
                t = q;
                p = q;
            } else {
                p->pSibling = q;
                p = q;
            }
        }
    }
    return t;
}

// var_declaration | func_declaration
Node* Parser::declara() {
    Node* t = NULL; 
    if (tokens[lookAhead].tokenVal == LPAREN) {
        t = createStmtNode(FuncK);
    } else { 
        t = createStmtNode(Var_DeclK);
        if (t != NULL && tokens[lookAhead + 2].tokenVal == LMP) {
            t->pChildNode[0] = arrayDecla();
        } else if (t != NULL) {
            t->pChildNode[0] = typeNode();
            t->Attr.name = new char[strlen(tokens[lookAhead].attribute.stringVal)];
            std::strcpy(t->Attr.name, tokens[lookAhead].attribute.stringVal);
            match(ID);
        }
    }
    return t;
}

// VOID | INT
Node* Parser::typeNode() {
    Node* t = NULL;
    switch(tokens[lookAhead].tokenVal) {
        case VOID:
            t = createTypeNode(Void);
            break;
        case INT:
            t = createTypeNode(Integer);
            break;
        default:
            break;
    }
    return t;
}

// array declaration
Node* Parser::arrayDecla() {
    Node* t = createExpNode(ArrayK);
    if (t != NULL) {
        if (tokens[lookAhead].tokenVal == VOID) { t->ExpType = Void; match(VOID); } 
        else if (tokens[lookAhead].tokenVal == INT) { t->ExpType = Integer; match(INT); }
        else { report("unexpected token->", lookAhead); }
    }
    if (t != NULL && tokens[lookAhead].tokenVal == ID) {
        t->Attr.name = new char[strlen(tokens[lookAhead].attribute.stringVal)];
        std::strcpy(t->Attr.name, tokens[lookAhead].attribute.stringVal);
    }
    match(ID);
    match(LMP);
    // match NUM or expression
    if (t != NULL) t->pChildNode[1] = simpleExp();
    match(RMP);
    return t;
}

// statement_list
Node* Parser::stmtSequence() {
    Node* t = stateMent();
    Node* p = t;
    while ((tokens[lookAhead].tokenVal != ENDFILE) && (tokens[lookAhead].tokenVal != ELSE)) {
        Node* q;
        match(SEMI);
        q = stateMent();
        if (q != NULL) {
            if (t == NULL) {
                t = q;
                p = q;	// new parent
            } else {
                p->pSibling = q;
                p = q;
            }
        }
    }
    return t;
}

// Statement contains three kinds of sub-statment: IF, REPEAT and expression
Node* Parser::stateMent() {
    Node* tmp = NULL;
    switch (tokens[lookAhead].tokenVal) {
        case IF:
            tmp = seleStmt();
            break;
        case WHILE:
            tmp = iteraStmt();
            break;
        case ID:
            // syntax error
            tmp = expression();
            break;
        case RETURN:
            tmp = returnStmt();
            break;
        default:
            report("unexcepted token->", lookAhead);
            lookAhead++;
            break;
    }
    return tmp;
}

// if expression then statement_list else statement_list end
Node* Parser::seleStmt() {
    Node* t = createStmtNode(IfK);
    match(IF);
    match(LPAREN);
    if (t != NULL) t->pChildNode[0] = expStmt();
    match(RPAREN);
    if (t != NULL) t->pChildNode[1] = compStmt();
    return t;
}

// repeat statement_list until expression
Node* Parser::iteraStmt() {
    Node* t = createStmtNode(WhileK);
    match(WHILE);
    match(LPAREN);
    if (t != NULL) t->pChildNode[0] = expStmt();
    match(RPAREN);
    if (t != NULL) t->pChildNode[1] = compStmt();
    return t;
}

// { local_declarations statement_list }
Node* Parser::compStmt() {
    Node* t = createStmtNode(CompK);
    match(LLP);
    // local declarations and statement_list
    if (t != NULL) t->pChildNode[0] = localDecla();
    if (t != NULL) t->pChildNode[1] = stmtSequence();
    match(RLP);
    return t;
}

Node* Parser::localDecla() {
    Node* t = declara();
    Node* p = t;
    while ((tokens[lookAhead].tokenVal != ENDFILE) && (tokens[lookAhead].tokenVal != ELSE)) {
        Node* q;
        match(SEMI);
        q = declara();
        if (q != NULL) {
            if (t == NULL) {
                t = q;
                p = q;	// new parent
            } else {
                p->pSibling = q;
                p = q;
            }
        }
    }
    return t;
}

// Expression statment contains only tow kinds expression: expression ; | ;
Node* Parser::expStmt() {
    Node* t = expression();
    Node* p = t;
    while (tokens[lookAhead].tokenVal != ENDFILE) {
        Node* q;
        match(SEMI);
        q = expression();
        if (q != NULL) {
            if (t == NULL) {
                t = p = q;
            } else {
                p->pSibling = q;
                p = q;
            }
        }
    }
    return t;
}

// assign statement
Node* Parser::expression() {
    Node* t;
    if (tokens[lookAhead].tokenVal == ID && tokens[lookAhead].tokenVal != LPAREN) {
        // var
        t = createStmtNode(AssignK);
        if (t != NULL) t->pChildNode[0] = var();
        match(ASSIGN);
        if (t != NULL) t->pChildNode[1] = expression();
    } else if (tokens[lookAhead].tokenVal == LPAREN) {
        // last
        t = simpleExp();
        match(SEMI);
    }
    return t;
}

Node* Parser::var() {
    Node* t = createExpNode(IdK);
    match(ID);
    if (tokens[lookAhead].tokenVal == LMP) {
        match(LMP);
        if (t != NULL) t->pChildNode[0] = expression();
        match(RMP);
    }
    return t;
}

// Simple expression contains two kinds: additiveExp relop additiveExp | additiveExp
// relop: + | -
Node* Parser::simpleExp() {
    Node* t = additiveExp();
    if (isRelop(tokens[lookAhead].tokenVal)) {
        Node* p = createExpNode(OpK);
        if (p != NULL) {
            p->pChildNode[0] = t;
            p->Attr.op = tokens[lookAhead].tokenVal;
            match(tokens[lookAhead].tokenVal);
            p->pChildNode[1] = additiveExp();
            t = p;
        }
    } else {
        report("unexpected token->", lookAhead);
    }
    return t;
}

Node* Parser::additiveExp() {
    Node* t = term();
    while ((tokens[lookAhead].tokenVal == PLUS) || (tokens[lookAhead].tokenVal == MINUS)) {
        Node* p = createExpNode(OpK);
        if (p != NULL) {
            p->pChildNode[0] = t;
            p->Attr.op = tokens[lookAhead].tokenVal;
            t = p;
            match(tokens[lookAhead].tokenVal);
            t->pChildNode[1] = term();
        }
    }
    return t;
}

Node* Parser::term() {
    Node* t = factor();
    while ((tokens[lookAhead].tokenVal == TIMES) || (tokens[lookAhead].tokenVal == OVER)) {
        Node* p = createExpNode(OpK);
        if (p != NULL) {
            p->pChildNode[0] = t;
            p->Attr.op = tokens[lookAhead].tokenVal;
            t = p;
            match(tokens[lookAhead].tokenVal);
            p->pChildNode[1] = factor();
        }
    }
    return t;
}

Node* Parser::factor() {
    Node* t = NULL;
    switch (tokens[lookAhead].tokenVal) {
        case LPAREN:
            match(LPAREN); t = expression(); match(RPAREN);
            break;
        case ID:
            if (tokens[lookAhead + 1].tokenVal == LPAREN) t = call();
            else {
                t = createExpNode(IdK);
                if (t != NULL && tokens[lookAhead].tokenVal == ID) {
                    t->Attr.name = new char[strlen(tokens[lookAhead].attribute.stringVal)];
                    strcpy(t->Attr.name, tokens[lookAhead].attribute.stringVal);
                }
                match(ID);
            }
            
            break;
        case NUM:
            t = createExpNode(ConstK);
            if ((t != NULL) && (tokens[lookAhead].tokenVal == NUM)) {
                t->Attr.val = tokens[lookAhead].attribute.numVal;
            }
            match(NUM);
            break;
        default:
            // syntax error
            report("unexcepted token->", lookAhead);
            break;
    }
    return t;
}

Node* Parser::args() {
    Node* t = arglist();
    return t;
}

Node* Parser::arglist() {
    Node* t = expression();
    Node* p = t;
    while (tokens[lookAhead].tokenVal != ENDFILE) {
        Node* q;
        match(COM);
        q = expression();
        if (p != NULL) {
            if (t == NULL) {
                t = p = q;
            } else {
                p->pSibling = q;
                p = q;
            }
        }
    }
    return t;
}

/*
 Private Inner function
 */
Node* Parser::createStmtNode(EStmtKind kind) {
    Node* t = new Node();
    t->NodeKind = StmtK;
    t->KNode.stmt = kind;
    return t;
}

Node* Parser::createExpNode(EExpKind kind) {
    Node* t = new Node();
    t->NodeKind = ExpK;
    t->KNode.exp = kind;
    return t;
}

bool Parser::isRelop(const TokenType tokenType) {
    return tokenType >= 11 && tokenType <= 16;
}
