#pragma once

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <cstddef>

#include "errors.h"
#include "guide.h"

/**
 Precedence (prc) constants.
*/
const short g_lowest_prc = 0;
const short g_low_prc = 1;
const short g_high_prc = 2;
const short g_highest_prc = 3;

const char g_lexicon_delimiters[2] = {' ', ';'};

enum class MlTypes
{
    mlnum,
    mlnamespace,
    mlinfix,
    mlaffix,
    mlcircumfix,
    mleof
};

std::string get_type_name(const MlTypes& tp);

/**
  A morpheme is the smallest unit of meaning characterized by semantics.
 
  For instance, "1" is a morpheme that represents the number one.
  Morphemes curate it's own interactions with bound-morphemes.
  New morphemes are created from combining a given morpheme and
  "bound" morphemes whereas "bound" morphemes are invalid alone.
 
  In this instance the negative unary operator "-" is a bound
  morpheme that carries with it extra meaning to our prior morpheme "1".
  Note that -1 consits of two morphemes, a lexeme.
  
  Some affixes are only valid when they are followed by an interfix.
  I.e. `return 0` cannot be used as `return0` since the interfix character, a whitespace
  plays a role on it's validation. 
  This is significant if the affix name is allowed to be used within variable names.
  The keyword `return` without this configuration of validation would mean that `return0` is looked
  at as an operation statement rather than a namespace thus restricting any namespaces hinting the affix name
*/
struct Morpheme
{
    std::string characters;
    MlTypes typehint;
    short precedence;
    bool hyphenated;
    bool isolated;
    bool unary;
    bool binary;

    bool operator==(Morpheme &other);

    bool operator!=(Morpheme &other);
};

/**
  A Token is a simple container with a meaning (morpheme) and it's position in
  the source code.
*/
struct Token
{
    Morpheme morpheme;
    Position position;
    bool valid;

    Token() : valid(false) {};

    Token(Position position)
        : position(position), valid(false) {};

    Token(Morpheme means, Position pos)
        : morpheme(means), position(pos), valid(true) {};

    bool operator==(Token& other);

    bool operator!=(Token& other);

    std::string to_string() const;

    friend std::ostream& operator<<(std::ostream& os, const Token& n);
};

Morpheme mlNumInfix(int stack, const Morpheme& op, const Morpheme& operand);

Morpheme mlNumInfix(int stack, const Morpheme& op);

Token mlNumConclude(TypeGuide& guide);

Morpheme mlNamespaceInfix(int stack, const Morpheme& op, const Morpheme& operand);

Morpheme mlNamespaceInfix(int stack, const Morpheme& op);

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

/*
    std::string characters;
    MlTypes typehint;
    short precedence;
    bool hyphenated;
    bool isolated;
    bool unary;
    bool binary;
  */

const Morpheme g_lexicon_affixes[12] =
{ {"~", MlTypes::mlinfix, g_lowest_prc, false},
  {"=", MlTypes::mlinfix, g_lowest_prc, false},
  {"+", MlTypes::mlinfix, g_lowest_prc, false},
  {"-", MlTypes::mlinfix, g_lowest_prc, false},
  {"*", MlTypes::mlinfix, g_low_prc, false },
  {"/", MlTypes::mlinfix, g_low_prc, false},
  {"...", MlTypes::mlinfix, g_low_prc, false},
  {"return", MlTypes::mlinfix, g_low_prc, false, true},
  {"{", MlTypes::mlcircumfix, g_lowest_prc},
  {"}", MlTypes::mlcircumfix, g_lowest_prc},
  {"(", MlTypes::mlcircumfix, g_lowest_prc},
  {")", MlTypes::mlcircumfix, g_lowest_prc} };
