#pragma once
#include <vector>
#include <memory>

#include "Lexer.h"

/**
 * @brief A lexeme a singular or a combination of morphemes, lexemes that look-a-like does not change
 * meaning.
 *
 * For instance -1  is a lexeme that represents a negative number one.
 * 1+2 is a lexeme that represents a positive number one being added to a positive number 2.
 *
 * 1 alone can also be a lexeme in some situations. All lexemes has (or are) morphemes but not all
 * morphemes can be lexemes.
*/

struct Operable
{
    Morpheme virtual eval() = 0;
    std::string virtual to_string()
    {
        return "n/a";
    };
};

struct Node : public Operable
{
    Token value;

    Node(Token &val) : value(val) {};

    Morpheme eval() override
    {
        return this->value.meaning;
    }

    std::string to_string() override
    {
        return this->value.meaning.to_string();
    };
};

struct BiNode : public Operable
{
    std::unique_ptr<Operable> &superior;
    Node op_token;
    std::unique_ptr<Operable> &inferior;

    BiNode(std::unique_ptr<Operable> &super, Node &op, std::unique_ptr<Operable> &infer) : superior(super), op_token(op), inferior(infer) {};

    Morpheme eval() override
    {
        return this->superior->eval().infix(this->op_token.value.meaning, inferior->eval(), 0);
    };

    std::string to_string() override
    {
        return '(' + this->superior->to_string() + this->op_token.to_string() + this->inferior->to_string() + ')';
    };
};

struct Parser
{
    std::vector<Token> tokens;
    Token cur_token;
    int index = 0;

    Parser(std::vector<Token> tks)
        : tokens(tks), cur_token(tks[0]) {};

    void update();

    void advance();

    void retreat();

    std::vector<std::unique_ptr<Operable>> exact();

    std::unique_ptr<Operable> deduceStatement(int prc);
};
