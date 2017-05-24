//
//  main.cpp
//  CompilerForMinus
//
//  Created by Andrew on 2017/4/23.
//  Copyright © 2017年 AK Inc. All rights reserved.
//

#include <iostream>
#include "Lexer.hpp"
#include "Parser.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::string example = "/Users/zhouming/GitHub/Compilers/CompilerForMinus/CompilerForMinus/CMinus/example.c-";
    Lexer lexerMode(example);
    lexerMode.lexer();
    lexerMode.show();
//    Parser parseMoudle(lexerMode.getTokenList());
//    parseMoudle.parser();
//    parseMoudle.show();
    return 0;
}
