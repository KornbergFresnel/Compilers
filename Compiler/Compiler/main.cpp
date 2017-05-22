//
//  main.cpp
//  Compiler
//
//  Created by Andrew on 2017/4/18.
//  Copyright © 2017年 AK Inc. All rights reserved.
//

#include <iostream>
#include "Lexer.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::string test = "/Users/zhouming/Downloads/Lab1/Tinytest/test.tiny";
    std::string example = "/Users/zhouming/Downloads/Lab1/Tinytest/example.tiny";
    Lexer lexerMode(test);
    lexerMode.lexer();
    lexerMode.show();
    lexerMode.store();
    return 0;
}
