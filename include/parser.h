#pragma once

#include <vector>
#include <memory>
#include <ostream>

#include "lexer.h"

/**
  A lexeme a singular or a combination of morphemes, lexemes that look-a-like does not change
  meaning.
 
  For instance -1  is a lexeme that represents a negative number one.
  1+2 is a lexeme that represents a positive number one being added to a positive number 2.
 
  1 alone can also be a lexeme in some situations. All lexemes has (or are) morphemes but not all
  morphemes can be lexemes.
*/

struct Operable
{
    Morpheme virtual eval() const = 0;
    std::string virtual to_string() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Operable& n);
};

struct Node : public Operable
{
    Token token;

    Node(Token tk) : token(tk){};

    Morpheme eval() const override;

    std::string to_string() const override;
};

struct BiNode : public Operable
{
    std::unique_ptr<Operable> superior;
    Node op_node;
    std::unique_ptr<Operable> inferior;

    BiNode(std::unique_ptr<Operable> super, Node& op, std::unique_ptr<Operable> infer)
        : superior(std::move(super)), op_node(op), inferior(std::move(infer)){};

    Morpheme eval() const;

    std::string to_string() const;
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

    std::vector<std::unique_ptr<Operable>> ast();

    std::unique_ptr<Operable> deduce_statement(int prc);
};
