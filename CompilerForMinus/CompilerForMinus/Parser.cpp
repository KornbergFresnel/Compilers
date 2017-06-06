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

void Parser::printStmtNode(Node* node, int layer) {
    switch (node->KNode.stmt) {
        case IfK:
            printf("If\n");
            break;
        case WhileK:
            printf("While\n");
            break;
        case AssignK:
            printf("Assign\n");
            break;
        case CompK:
            printf("CompK\n");
            break;
        case CallK:
            printf("CallK\n");
            break;
        default:
            break;
    }
    printTree(node->pChildNode[0], layer + 1);
    printTree(node->pChildNode[1], layer + 1);
    if (node->KNode.stmt == IfK) printTree(node->pChildNode[2], layer + 1);
}

void Parser::printExpNode(Node* node, int layer) {
    switch (node->KNode.exp) {
        case OpK:
            printOp(node->Attr.op);
            printTree(node->pChildNode[0], layer + 1);
            printTree(node->pChildNode[1], layer + 1);
            break;
        case ConstK:
            printf("ConstK: %d\n", node->Attr.val);
            break;
        case IdK:
            printf("IdK: %s\n", node->Attr.name);
            break;
        case Arry_ElemK:
            printf("Arry_ElemK\n");
            printTree(node->pChildNode[0], layer + 1);
            printTree(node->pChildNode[1], layer + 1);
            break;
        case ReturnK:
            printf("Return\n");
            printTree(node->pChildNode[0], layer + 1);
            break;
        default:
            break;
    }
}

void Parser::printDeclaNode(Node* node, int layer) {
    switch (node->KNode.decla) {
        case Var_DeclK:
            printf("Var_DeclK\n");
            printTree(node->pChildNode[0], layer + 1);
            printTree(node->pChildNode[1], layer + 1);
            break;
        case Arry_DeclK:
            printf("Arry_DeclK\n");
            printTree(node->pChildNode[0], layer + 1);
            printTree(node->pChildNode[1], layer + 1);
            break;
        case Funck:
            printf("Funck\n");
            printTree(node->pChildNode[0], layer + 1);
            printTree(node->pChildNode[1], layer + 1);
            printTree(node->pChildNode[2], layer + 1);
            printTree(node->pChildNode[3], layer + 1);
            break;
        case ParamsK:
            printf("ParamsK\n");
            printTree(node->pChildNode[0], layer + 1);
            break;
        case VoidK:
            printf("VoidK\n");
            printTree(node->pChildNode[0], layer + 1);
            break;
        case ParamK:
            printf("Paramk\n");
            printTree(node->pChildNode[0], layer + 1);
            printTree(node->pChildNode[1], layer + 1);
            break;
        default:
            break;
    }
}

void Parser::printTypeNode(Node* node) {
    switch (node->ExpType) {
        case Void:
            printf("VoidK\n");
            break;
        case Integer:
            printf("IntK\n");
            break;
        default:
            break;
    }
}

// Print tree
void Parser::printTree(Node *node, int layer) {
    if (NULL == node) return;
    for (int i = 0; i < layer; i++) printf("\t");
    if (node->NodeKind == StmtK) printStmtNode(node, layer);
    else if (node->NodeKind == ExpK) printExpNode(node, layer);
    else if (node->NodeKind == DeclaK) printDeclaNode(node, layer);
    else if (node->NodeKind == TypeK) printTypeNode(node);
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
    else { report("match: unexcepted token->", lookAhead); lookAhead++; }
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
    // Node* t = createDecl();
    Node* t = createDeclNode(Var_DeclK);
    t->pChildNode[0] = typeDefine();    // type

    if (tokens[lookAhead].tokenVal == ID) {
        Node* p = createExpNode(IdK);
        p->Attr.name = new char[strlen(tokens[lookAhead].attribute.stringVal)];
        std::strcpy(p->Attr.name, tokens[lookAhead].attribute.stringVal);
        match(ID);
        t->pChildNode[1] = p;   // name
    } else {
        report("declara: unexpected token->", lookAhead);
        t = NULL;
        lookAhead++;
    }
    
    if (t != NULL && tokens[lookAhead].tokenVal == LMP) {
        // create a Arry_DeclK
        match(LMP);
        Node* p = createDeclNode(Arry_DeclK);        
        if (tokens[lookAhead].tokenVal == NUM) {
            p->pChildNode[0] = createExpNode(IdK);
            p->pChildNode[0]->Attr.name = t->pChildNode[1]->Attr.name;
            t->pChildNode[1] = NULL;
            p->pChildNode[1] = createExpNode(ConstK);
            p->pChildNode[1]->Attr.val = tokens[lookAhead].attribute.numVal;
        }
        t->pChildNode[1] = p;
        match(NUM); match(RMP);
    }
    if (tokens[lookAhead].tokenVal == SEMI) match(SEMI);
    if (t != NULL && tokens[lookAhead].tokenVal == LPAREN) {
        t->KNode.decla = Funck; // modify decla type
        match(LPAREN);
        t->pChildNode[2] = params();
        match(RPAREN);
        t->pChildNode[3] = compStmt();
    }
    return t;
}


Node* Parser::varDecla() {
    Node* t = NULL;
    if (tokens[lookAhead].tokenVal == INT || tokens[lookAhead].tokenVal == VOID) {
        t = createDeclNode(Var_DeclK);
        t->pChildNode[0] = typeDefine();
    } else return t;
    
    if (tokens[lookAhead].tokenVal == ID) {
        Node* p = createExpNode(IdK);
        p->Attr.name = new char[strlen(tokens[lookAhead].attribute.stringVal)];
        std::strcpy(p->Attr.name, tokens[lookAhead].attribute.stringVal);
        match(ID);
        t->pChildNode[1] = p;   // name
    } else {
        report("varDecla: unexpected token->", lookAhead);
        t = NULL;
        lookAhead++;
    }
    
    if (t != NULL && tokens[lookAhead].tokenVal == LMP) {
        // create a Arry_DeclK
        Node* p = createDeclNode(Arry_DeclK);
        p->Attr.name = t->Attr.name;
        t->Attr.name = NULL;
        t->pChildNode[1] = p;   // modify to array, and set array's name with current name
        
        if (tokens[lookAhead].tokenVal == NUM) {
            t->pChildNode[2] = createExpNode(ConstK);
            t->pChildNode[2]->Attr.val = tokens[lookAhead].attribute.numVal;
        }
        match(NUM); match(RMP);
    }
    match(SEMI);
    return t;
}

Node* Parser::typeDefine() {
    Node* t = NULL;
    if (tokens[lookAhead].tokenVal == INT) {
        t = new Node();
        t->NodeKind = TypeK;
        t->ExpType = Integer;
        match(INT);
    } else if (tokens[lookAhead].tokenVal == VOID) {
        t = new Node();
        t->NodeKind = TypeK;
        t->ExpType = Void;
        match(VOID);
    } else {
        report("type: Unexpected token->", lookAhead);
        lookAhead++;
    }
    return t;
}

Node* Parser::params() {
    Node* t = createDeclNode(ParamsK);
    if (tokens[lookAhead].tokenVal == VOID) {
        t->pChildNode[0] = createDeclNode(VoidK);
        match(VOID);
    } else t->pChildNode[0] = paramlist();
    return t;
}

Node* Parser::paramlist() {
    Node* t = param();
    Node* p = t;
    while (tokens[lookAhead].tokenVal != ENDFILE && tokens[lookAhead].tokenVal != RPAREN) {
        Node* q;
        match(COM);
        q = param();
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

// BUG
Node* Parser::param() {
    // match type
    // match ID
    Node* t = createDeclNode(ParamK);
    t->pChildNode[0] = typeDefine();

    if (tokens[lookAhead].tokenVal == ID) {
        Node* p = createExpNode(IdK);
        p->Attr.name = new char[strlen(tokens[lookAhead].attribute.stringVal)];
        std::strcpy(p->Attr.name, tokens[lookAhead].attribute.stringVal);
        match(ID);
        t->pChildNode[1] = p;   // name
    } else {
        report("param: unexpected token->", lookAhead);
        t = NULL;
        lookAhead++;
    }
    // param may be array
    if (tokens[lookAhead].tokenVal == LMP) {
        match(LMP);
        match(RMP);
    }
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
    Node* t = varDecla();
    Node* p = t;
    while (tokens[lookAhead].tokenVal == INT || tokens[lookAhead].tokenVal == VOID) {
        Node* q;
        q = varDecla();
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

// statement_list
Node* Parser::stmtSequence() {
    Node* t = stateMent();
    Node* p = t;
    while (tokens[lookAhead].tokenVal != ENDFILE && tokens[lookAhead].tokenVal != RLP && tokens[lookAhead].tokenVal != ELSE) {
        Node* q;
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
            tmp = expStmt();
            break;
        case LLP:
            tmp = compStmt();
            break;
        case RETURN:
            tmp = returnStmt();
            break;
        default:
            report("statement: unexcepted token->", lookAhead);
            lookAhead++;
            break;
    }
    return tmp;
}

// Expression statment contains only tow kinds expression: expression ; | ;
Node* Parser::expStmt() {
    Node* t = expression();
    Node* p = t;
    while (tokens[lookAhead].tokenVal == SEMI) {
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

// if expression then statement_list else statement_list end
Node* Parser::seleStmt() {
    Node* t = createStmtNode(IfK);
    match(IF);
    match(LPAREN);
    if (t != NULL) t->pChildNode[0] = expression();
    match(RPAREN);
    if (t != NULL) t->pChildNode[1] = compStmt();
    if (tokens[lookAhead].tokenVal == ELSE) {
        match(ELSE);
        if (t != NULL) t->pChildNode[2] = compStmt();
    }
    return t;
}

// repeat statement_list until expression
Node* Parser::iteraStmt() {
    Node* t = createStmtNode(WhileK);
    match(WHILE);
    match(LPAREN);
    if (t != NULL) t->pChildNode[0] = expression();
    match(RPAREN);
    if (t != NULL) t->pChildNode[1] = compStmt();
    return t;
}

Node* Parser::returnStmt() {
    Node* t = createExpNode(ReturnK);
    match(RETURN);
    t->pChildNode[0] = expression();
    match(SEMI);
    return t;
}

// assign statement
Node* Parser::expression() {
    Node* t = NULL;
    if (tokens[lookAhead].tokenVal == ID) {
        // var
        t = createStmtNode(AssignK);
        size_t oldLookAhead = lookAhead;
        t->pChildNode[0] = var();
        if (t->pChildNode[0] == NULL) {
            // must be a simple
            delete t;
            t = simpleExp();
        } else if (tokens[lookAhead].tokenVal == LPAREN || isOperator(tokens[lookAhead].tokenVal)) {
            delete t;
            lookAhead = oldLookAhead;
            t = simpleExp();
        } else if (tokens[lookAhead].tokenVal == ASSIGN) {
            match(ASSIGN);
            t->pChildNode[1] = expression();
        }
    } else if (tokens[lookAhead].tokenVal == NUM || tokens[lookAhead].tokenVal == LPAREN) t = simpleExp();
    return t;
}

Node* Parser::var() {
    Node* t = createExpNode(IdK);
    if (tokens[lookAhead].tokenVal == ID) {
        t->Attr.name = new char[strlen(tokens[lookAhead].attribute.stringVal)];
        std::strcpy(t->Attr.name, tokens[lookAhead].attribute.stringVal);
    }
    match(ID);
    if (tokens[lookAhead].tokenVal == LMP) {
        Node* p = createExpNode(Arry_ElemK);
        p->pChildNode[0] = createExpNode(IdK);
        p->pChildNode[0]->Attr.name = new char[strlen(t->Attr.name)];
        std::strcpy(p->pChildNode[0]->Attr.name, t->Attr.name);
        delete t;
        t = p;
        match(LMP);
        t->pChildNode[1] = expression();
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
            else t = var();
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
            report("factor: unexcepted token->", lookAhead);
            lookAhead++;
            break;
    }
    return t;
}

Node* Parser::call() {
    Node* t = createStmtNode(CallK);
    if (tokens[lookAhead].tokenVal == ID) {
        Node* p =createExpNode(IdK);
        p->Attr.name = new char[strlen(tokens[lookAhead].attribute.stringVal)];
        std::strcpy(p->Attr.name, tokens[lookAhead].attribute.stringVal);
        t->pChildNode[0] = p;
    }
    match(ID);
    match(LPAREN);
    t->pChildNode[1] = args();
    match(RPAREN);
    return t;
}

Node* Parser::args() {
    Node* t = arglist();
    return t;
}

Node* Parser::arglist() {
    Node* t = expression();
    Node* p = t;
    while (tokens[lookAhead].tokenVal != ENDFILE && tokens[lookAhead].tokenVal != RPAREN) {
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

Node* Parser::createDeclNode(EDeclKind kind) {
    Node* t = new Node();
    t->NodeKind = DeclaK;
    t->KNode.decla = kind;
    return t;
}

bool Parser::isRelop(const TokenType tokenType) {
    return tokenType >= 11 && tokenType <= 16;
}

bool Parser::isOperator(const TokenType tokenType) {
    return tokenType >= 11 && tokenType <= 20;
}
