#pragma once

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <cstddef>

#include "errors.hpp"
#include "guide.hpp"
#include "types.hpp"

/**
 Precedence (prc) constants.
*/
const short g_lowest_prc = 0;
const short g_low_prc = 1;
const short g_high_prc = 2;
const short g_highest_prc = 3;

const char g_lexicon_delimiters[2] = {' ', ';'};

/**
 * mlnum       : Numeric type, every numeric string of characters
 * mlnamespace : Namespace type, every literal string of characters until it is resolved
 *               into it's real type
 * mlaffix     : Affix type, an exhaustible built-in or user defined type of a literal string of characters
 * mlprefix    : Prefix type, an exhustible built-in or user defined type of an affix that is found pre statement
 * mlinfix     : Infix type, an exhustible built-in or user defined type of an affix that is found in between a statement
 * mlcircumfix : Circumfix type, an exhustible built-in or user defined type of an affix that is found surrounding
 *               the statement at the start and the end
 * mleof       : EOF type, remarks the end of a token stream.
*/
enum class MlTypes
{
    mlnum,
    mlnamespace,
    mlaffix,
    mlprefix,
    mlinfix,
    mlcircumfix,
    mlunknown,
    mleof
};

std::string get_type_name(const MlTypes& tp);

/**
 * characters : the string representation of the lexeme
 * typehint   : the type representation of the lexeme in an enum value
 * precedence : the precedence of the lexeme compartively to other lexemes
 * hyphenated : whether if this lexeme needs a hyphen to be valid
 * isolated   : whether if the lexeme needs to be surrounded by delimiters thus isolated
 *              to be considered valid
 * unary      : whether if the lexeme responds to unary calls to be an operator
 * binary     : whether if the lexeme responds to binary calls to be an operator
*/
struct Lexeme
{
    std::string characters;
    MlTypes typehint = MlTypes::mlunknown;
    short precedence = g_lowest_prc;
    bool hyphenated = false;
    bool isolated = false;
    bool unary = false;
    bool binary = false;

    bool operator==(Lexeme &other);

    bool operator!=(Lexeme &other);
};

/**
  A Token is a simple container with a meaning (morpheme) and it's position in
  the source code.
*/
struct Token
{
    Lexeme lexeme;
    Position position;
    bool valid;

    Token() : valid(false) {};

    Token(Position position)
        : position(position), valid(false) {};

    Token(Lexeme means, Position pos)
        : lexeme(means), position(pos), valid(true) {};

    bool operator==(Token& other);

    bool operator!=(Token& other);

    std::string to_string() const;

    friend std::ostream& operator<<(std::ostream& os, const Token& n);
};

Token mlNumConclude(TypeGuide& guide);

Token mlNamespaceConConclude(TypeGuide& guide, std::string result, Position pos);

Token mlNamespaceConclude(TypeGuide& guide);


struct LexxedResult
{
    std::unique_ptr<BaseException> error;
    std::vector<Token> tokens;
    bool failed;

    LexxedResult(std::vector<Token>& tks)
        : tokens(tks), failed(false) {};

    LexxedResult(std::unique_ptr<BaseException>& fault)
        : error(std::move(fault)), failed(true) {}
};


LexxedResult tokenize();

/* std::string characters;
   MlTypes typehint;
   short precedence;
   bool hyphenated;
   bool isolated;
   bool unary;
   bool binary; */

const Lexeme g_lexicon_affixes[21] =
{ {"...", MlTypes::mlaffix, g_low_prc, false, false, false, false},
  {"\"", MlTypes::mlaffix, g_low_prc, false, false, false, false},
  {"~", MlTypes::mlprefix, g_lowest_prc, false, false, true, false},
  {"return", MlTypes::mlprefix, g_low_prc, false, true, true, false},
  {"str", MlTypes::mlprefix, g_low_prc, false, true, true, false},
  {"mstr", MlTypes::mlaffix, g_low_prc, false, true, true, false},
  {"int", MlTypes::mlprefix, g_low_prc, false, true, true, false},
  {"float", MlTypes::mlprefix, g_low_prc, false, true, true, false},
  {":", MlTypes::mlinfix, g_lowest_prc, false, false, false, true},
  {"if", MlTypes::mlprefix, g_low_prc, false, true, true, false},
  {",", MlTypes::mlinfix, g_lowest_prc, false, false, false, true},
  {"=", MlTypes::mlinfix, g_lowest_prc, false, false, false, true},
  {"+", MlTypes::mlinfix, g_lowest_prc, false, false, false, true},
  {".", MlTypes::mlinfix, g_lowest_prc, false, false, false, true},
  {"-", MlTypes::mlinfix, g_lowest_prc, false, false, true, true},
  {"*", MlTypes::mlinfix, g_low_prc, false, false, false, true},
  {"/", MlTypes::mlinfix, g_low_prc, false, false, false, true},
  {"{", MlTypes::mlcircumfix, g_lowest_prc, false, false, false, false},
  {"}", MlTypes::mlcircumfix, g_lowest_prc, false, false, false, false},
  {"(", MlTypes::mlcircumfix, g_lowest_prc, false, false, false, false},
  {")", MlTypes::mlcircumfix, g_lowest_prc, false, false, false, false} };