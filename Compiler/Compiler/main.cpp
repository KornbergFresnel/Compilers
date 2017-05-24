//
//  main.cpp
//  Compiler
//
//  Created by Andrew on 2017/4/18.
//  Copyright © 2017年 AK Inc. All rights reserved.
//

#include <iostream>
#include "Lexer.hpp"
#include "Parser.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::string test = "/Users/zhouming/GitHub/Compilers/Compiler/Tinytest/test.tiny";
    std::string example = "/Users/zhouming/GitHub/Compilers/Compiler/Tinytest/example.tiny";
    Lexer lexerMode(example);
    lexerMode.lexer();
    // lexerMode.show();
    Parser parserModule(lexerMode.getTokenList());
    parserModule.parser();
    parserModule.show();
    return 0;
}
