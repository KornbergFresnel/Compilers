//
//  main.cpp
//  CompilerForMinus
//
//  Created by Andrew on 2017/4/23.
//  Copyright © 2017年 AK Inc. All rights reserved.
//

#include <iostream>
#include "Lexer.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::string test = "/Users/zhouming/Downloads/Lab1/Tinytest/test.tiny";
    std::string example = "/Users/zhouming/Downloads/Lab2/example.c-";
    Lexer lexerMode(example);
    lexerMode.lexer();
    lexerMode.show();
    lexerMode.store();
    return 0;
}
