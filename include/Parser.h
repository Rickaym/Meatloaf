#pragma once
#include <vector>

#include "Lexer.h"

struct Node
{
};

struct Parser
{
    std::vector<Token> tokens;
    int index;
    Token &cur_token;

    Parser(std::vector<Token> tks)
        : tokens(tks), cur_token(tks[0]){};

    std::vector<Node> ast();

    static Node deduceStatement(Token &tk);
};