#pragma once
#include <vector>

#include "Lexer.h"

/**
 * @brief A lexeme a singular or a combination of morphemes does not change it's meaning.
 *
 * For instance -1  is a lexeme that represents a negative number one.
 * 1+2 is a lexeme that represents a positive number one being added to a positive number 2.
 *
 * 1 alone can also be a lexeme in some situations. All lexemes has (or are) morphemes but not all
 * morphemes can be lexemes.
*/
struct Lexeme
{
};

struct Parser
{
    std::vector<Token> tokens;
    int index;
    Token &cur_token;

    Parser(std::vector<Token> tks)
        : tokens(tks), cur_token(tks[0]){};

    std::vector<Lexeme> ast();

    static Lexeme deduceStatement(Token &tk);
};