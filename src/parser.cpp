#include <vector>
#include <iostream>
#include <memory>

#include "lexer.h"
#include "parser.h"

std::ostream& operator<<(std::ostream& os, const Operable& n)
{
    os << n.to_string();
    return os;
}

void Parser::update()
{
    if (this->index < (int)this->tokens.size())
    {
        this->cur_token = this->tokens[this->index];
    };
};

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

Morpheme Node::eval() const 
{
    return this->token.meaning;
}

std::string Node::to_string() const
{
    return this->token.meaning.to_string();
};

Morpheme BiNode::eval() const 
{
    return this->superior->eval().infix(this->op_node.token.meaning, inferior->eval(), 0);
};

std::string BiNode::to_string() const 
{
    return '(' + this->superior->to_string() + ' ' + this->op_node.to_string() + ' ' + this->inferior->to_string() + ')';
};


std::vector<std::unique_ptr<Operable>> Parser::ast()
{
    std::vector<std::unique_ptr<Operable>> nodes;
    nodes.push_back(this->deduce_statement(0));
    return nodes;
};

std::unique_ptr<Operable> Parser::deduce_statement(int prc)
{
    if (prc == 6)
    {
        return std::make_unique<Node>(this->cur_token);
    };
    std::unique_ptr<Operable> superior = this->deduce_statement(prc + 1);
    this->advance();
    Node op = this->cur_token;
    while (op.token.meaning.precedence == prc && op.token.meaning.value != "N/A")
    {
        this->advance();
        std::unique_ptr<Operable> inferior = this->deduce_statement(prc + 1);
        superior = std::make_unique<BiNode>(std::move(superior), op, std::move(inferior));
        this->advance();
        op = this->cur_token;
    };
    this->retreat();
    return superior;
};
