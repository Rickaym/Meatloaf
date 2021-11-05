#pragma once
#include <vector>
#include <memory>

#include "lexer.h"

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
    std::string virtual to_string() = 0;
};

struct Node : public Operable
{
    Token token;

    Node(Token &tk) : token(tk){};

    Morpheme eval() override
    {
        return this->token.meaning;
    }

    std::string to_string() override
    {
        return this->token.meaning.to_string();
    };
};

struct BiNode : public Operable
{
    Operable& superior;
    Node op_node;
    Operable& inferior;

    BiNode(Operable& super, Node& op, Operable& infer) 
        : superior(super), op_node(op), inferior(infer){};

    Morpheme eval() override
    {
        return this->superior.eval().infix(this->op_node.token.meaning, inferior.eval(), 0);
    };

    std::string to_string() override
    {
        return '(' + this->superior.to_string() + ' ' + this->op_node.to_string() + ' ' + this->inferior.to_string() + ')';
    };
};

struct Parser
{
    std::vector<Token> tokens;
    Token cur_token;
    int index = 0;

    Parser(std::vector<Token> tks)
        : tokens(tks), cur_token(tks[0]){};

    void update();

    void advance();

    void retreat();

    std::vector<Operable*> exact();

    Operable* deduceStatement(int prc);
};
