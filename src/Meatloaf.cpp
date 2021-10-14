#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>

#include "Lexer.h"

int main() {
    std::cout << "Welcome to the Meatloaf interactive grill\n Get ready to make your meat and we'll bread em.\nVersion 0.0.0.a\n" << std::endl;
    std::string cmdlet;
    while (true) {
        std::cout << "->- ";
        std::getline(std::cin, cmdlet);

        std::vector<Token> tokens = Lexer::tokenize(cmdlet);
        std::cout << "... ";
        for (Token tk: tokens)
        {
            std::cout << tk.toString() << ' ';
        };
        std::cout << tokens.size() << '\n';
    };
    std::cout << std::endl;
    return 0;
};

