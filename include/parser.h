#pragma once

#include <vector>
#include <memory>
#include <ostream>

#include "lexer.h"
#include "errors.h"

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

struct Task
{
    bool failed;
    std::unique_ptr<Operable> value;
    std::unique_ptr<BaseException> error;

    Task() = default;

    void success(std::unique_ptr<Operable>&& val);

    void success(std::unique_ptr<Operable>& val);

    void failure(std::unique_ptr<BaseException>&& fault);

    void failure(std::unique_ptr<BaseException>& fault);
};

struct Node : public Operable
{
    Token token;

    Node(Token& tk) : token(std::move(tk)) {};

    Morpheme eval() const override;

    std::string to_string() const override;
};

struct BiNode : public Operable
{
    std::unique_ptr<Operable> superior;
    Node op_node;
    std::unique_ptr<Operable> inferior;

    BiNode(std::unique_ptr<Operable> super, Node& op, std::unique_ptr<Operable> infer)
        : superior(std::move(super)), op_node(std::move(op)), inferior(std::move(infer)){};

    Morpheme eval() const;

    std::string to_string() const;
};

struct ParsedResult
{
    bool failed;
    std::vector<std::unique_ptr<Operable>> nodes;
    std::unique_ptr<BaseException> error;

    ParsedResult(std::vector<std::unique_ptr<Operable>> nds)
        : nodes(std::move(nds)), failed(false) {};

    ParsedResult(std::unique_ptr<BaseException> fault)
        : error(std::move(fault)), failed(true) {};

};


struct Parser
{
    std::vector<Token> tokens;
    int index = 0;

    Parser(std::vector<Token>& tks)
        : tokens(std::move(tks)) {};

    Token& cur_token();

    void advance();

    void retreat();

    ParsedResult ast();

    void Parser::deduce_statement(int prc, Task& tsk);
};


