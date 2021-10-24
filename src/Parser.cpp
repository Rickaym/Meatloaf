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

std::vector<std::unique_ptr<Operable>> Parser::exact()
{
    std::vector<std::unique_ptr<Operable>> nodes;
    nodes.push_back(this->deduceStatement(0));
    return nodes;
};

std::unique_ptr<Operable> Parser::deduceStatement(int prc)
{
    if (prc == 6)
    {
        std::cout << "RLS CURTK " << this->cur_token.to_string() << "\n";
        return std::unique_ptr<Operable> {new Node(this->cur_token)};
    };
    std::unique_ptr<Operable> superior = this->deduceStatement(prc+1);
    this->advance();
    Node op = this->cur_token;
    while (op.value.meaning.precedence == prc && op.value.meaning.value != "N/A") {
        this->advance();
        std::unique_ptr<Operable> inferior = this->deduceStatement(prc+1);
        superior = std::unique_ptr<Operable> {new BiNode(superior, op, inferior)};
        this->advance();
        op = this->cur_token;
    };
    this->retreat();
    std::cout << "LEAVE " << prc << "\n";
    return superior;
};
