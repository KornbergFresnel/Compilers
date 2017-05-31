//
//  main.cpp
//  Compiler
//
//  Created by Andrew on 2017/4/18.
//  Copyright © 2017年 AK Inc. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <cstring>
#include "Lexer.hpp"
#include "Parser.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
	std::string info = "Usage: ./compiler -h [--help]\n\t-l [filename]\t--for running lexer\n\t-p [filename]\t--for running parser";
	if (argc <= 2) {
		std::cout << info << std::endl;
	} else {
		if (std::strcmp(argv[1], "-l") == 0) {
			std::string file(argv[2]);
			Lexer lexerMode(file);
			lexerMode.lexer();
			lexerMode.show();
		} else if (std::strcmp(argv[1], "-p") == 0) {
			std::string file(argv[2]);
			Lexer lexerMode(file);
			lexerMode.lexer();
			Parser parseMoudle(lexerMode.getTokenList());
			parseMoudle.parser();
			parseMoudle.show();
		} else if (std::strcmp(argv[1], "-h") == 0) {
			std::cout << info << std::endl;
		}
	}
    return 0;
}
