#include <vector>
#include <iostream>
#include <memory>

#include "Lexer.h"
#include "Parser.h"

void Parser::update()
{
    if (this->index < (int)this->tokens.size())
    {
        this->cur_token = this->tokens[this->index];
    };
}

void Parser::advance()
{
    this->index++;
    this->update();
};

void Parser::retreat()
{
    this->index--;
    this->update();
};

std::vector<Operable*> Parser::exact()
{
    std::vector<Operable*> nodes;
    nodes.push_back(this->deduceStatement(0));
    return nodes;
};

Operable* Parser::deduceStatement(int prc)
{
    if (prc == 6)
    {
        return new Node(this->cur_token);
    };
    Operable* superior = this->deduceStatement(prc + 1);
    this->advance();
    Node op = this->cur_token;
    while (op.token.meaning.precedence == prc && op.token.meaning.value != "N/A")
    {
        this->advance();
        Operable* inferior = this->deduceStatement(prc + 1);
        // TODO: free da pointers
        superior = new BiNode(*superior, op, *inferior);
        this->advance();
        op = this->cur_token;
    };
    this->retreat();
    return superior;
};
