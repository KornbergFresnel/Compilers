//
//  Parser.cpp
//  Compiler
//
//  Created by Andrew on 2017/5/2.
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
	syntaxTree = stmtSequence();
	if (tokens[lookAhead].tokenVal != ENDFILE)
		report("Code ends before file\n", -1);
}

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
		case EQ:
			printf("Op: =\n");
			break;
		default:
			break;
	}
}

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
			case ReapeatK:
				printf("Repeat\n");
				printTree(node->pChildNode[0], layer + 1);
				printTree(node->pChildNode[1], layer + 1);
				break;
			case AssignK:
				printf("Assign to: %s\n", node->Attr.name);
				printTree(node->pChildNode[0], layer + 1);
				break;
			case ReadK:
				printf("Read: %s\n", node->Attr.name);
				break;
				// printTree(node->pChildNode[0], layer + 1);
			case WriteK:
				printf("Write\n");
				printTree(node->pChildNode[0], layer + 1);
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
	
	while (node->pSibling != NULL) {
		printTree(node->pSibling, layer);
		node = node->pSibling;
	}
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

Node* Parser::stmtSequence() {
	Node* t = stateMent();
	Node* p = t;
	while ((tokens[lookAhead].tokenVal != ENDFILE) && (tokens[lookAhead].tokenVal != END) &&
		   (tokens[lookAhead].tokenVal != ELSE) && (tokens[lookAhead].tokenVal != UNTIL)) {
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
        case REPEAT:
            tmp = iteraStmt();
            break;
        case WRITE:
			tmp = writeStmt();
            break;
		case READ:
			tmp = readStmt();
			break;
		case ID:
			// syntax error
			tmp = assignStmt();
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
	if (t != NULL) t->pChildNode[0] = expStmt();
	match(THEN);
	if (t != NULL) t->pChildNode[1] = stmtSequence();
	if (tokens[lookAhead].tokenVal == ELSE) {
		match(ELSE);
		if (t != NULL) t->pChildNode[2] = stmtSequence();
	}
	match(END);
	return t;
}

// repeat statement_list until expression
Node* Parser::iteraStmt() {
	Node* t = createStmtNode(ReapeatK);
	match(REPEAT);
	if (t != NULL) t->pChildNode[0] = stmtSequence();
	match(UNTIL);
	if (t != NULL) t->pChildNode[1] = expStmt();
	return t;
}

// assign statement
Node* Parser::assignStmt() {
	Node* t = createStmtNode(AssignK);
	if (t != NULL && tokens[lookAhead].tokenVal == ID) {
		t->Attr.name = new char[strlen(tokens[lookAhead].attribute.stringVal)];
		strcpy(t->Attr.name, tokens[lookAhead].attribute.stringVal);
	}
	match(ID);
	match(ASSIGN);
	if (t != NULL) t->pChildNode[0] = expStmt();
	return t;
}

Node* Parser::writeStmt() {
	Node* t = createStmtNode(WriteK);
	match(WRITE);
	if (t != NULL) t->pChildNode[0] = expStmt();
	return t;
}

Node* Parser::readStmt() {
	Node* t = createStmtNode(ReadK);
	match(READ);
	if ((t != NULL) && (tokens[lookAhead].tokenVal == ID)) {
		t->Attr.name = new char[strlen(tokens[lookAhead].attribute.stringVal)];
		std::strcpy(t->Attr.name, tokens[lookAhead].attribute.stringVal);
	}
	match(ID);
	return t;
}

// Expression statment contains only tow kinds expression: expression ; | ;
Node* Parser::expStmt() {
	Node* t = simpleExp();
	if (tokens[lookAhead].tokenVal == LT || tokens[lookAhead].tokenVal == EQ) {
		Node* p = createExpNode(OpK);
		if (p != NULL) {
			p->pChildNode[0] = t;
			p->Attr.op = tokens[lookAhead].tokenVal;
			t = p;
		}
		match(tokens[lookAhead].tokenVal);
		if (t != NULL) {
			t->pChildNode[1] = simpleExp();
		}
	}
	return t;
}

// Simple expression contains two kinds: additiveExp relop additiveExp | additiveExp
// relop: + | -
Node* Parser::simpleExp() {
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
            match(LPAREN); t = expStmt(); match(RPAREN);
            break;
        case ID:
			t = createExpNode(IdK);
			if ((t != NULL) && (tokens[lookAhead].tokenVal == ID)) {
				t->Attr.name = new char[strlen(tokens[lookAhead].attribute.stringVal)];
				strcpy(t->Attr.name, tokens[lookAhead].attribute.stringVal);
			}
            match(ID);
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
