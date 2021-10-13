#include <vector>

#include "Parser.h"

std::vector<Node> Parser::ast()
{
    while (this->cur_token != this->tokens.back())
    {
        this->deduceStatement(this->cur_token);
    };
};

Node Parser::deduceStatement(Token &tk)
{};