#include <vector>
#include <iostream>
#include <memory>

#include "lexer.h"
#include "parser.h"

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

std::vector<Operable*> Parser::ast()
{
    std::vector<Operable*> nodes;
    nodes.push_back(this->deduce_statement(0));
    return nodes;
};

Operable* Parser::deduce_statement(int prc)
{
    if (prc == 6)
    {
        return new Node(this->cur_token);
    };
    Operable* superior = this->deduce_statement(prc + 1);
    this->advance();
    Node op = this->cur_token;
    while (op.token.meaning.precedence == prc && op.token.meaning.value != "N/A")
    {
        this->advance();
        Operable* inferior = this->deduce_statement(prc + 1);
        // TODO: free da pointers
        superior = new BiNode(*superior, op, *inferior);
        this->advance();
        op = this->cur_token;
    };
    this->retreat();
    return superior;
};
