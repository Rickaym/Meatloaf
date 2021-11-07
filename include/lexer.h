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

enum Typehints
{
    mlnum,
    mlnamespace,
    mlinfix,
    mlaffix,
};


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
    Typehints typehint;
    bool valid;
    short precedence;
    bool hyphenated;
    bool isolated;
    bool unary;
    bool binary;

    Morpheme() = default;

    bool operator==(Morpheme &other);

    bool operator!=(Morpheme &other);

    std::string to_string() const;

protected:
    Morpheme(std::string crs, Typehints tph)
        : characters(crs), typehint(tph), valid(true) {};
    
    Morpheme(std::string crs, Typehints tph, short prc, bool hyp, bool islt, bool unary, bool binary)
        : characters(crs), typehint(tph), precedence(prc), hyphenated(hyp), isolated(islt), unary(unary), binary(binary) {};
};

/**
  A Token is a simple container with a meaning (morpheme) and it's position in
  the source code.
*/
struct Token
{
    std::unique_ptr<Morpheme> morpheme;
    Position position;
    bool valid;

    Token() : valid(false) {};

    Token(Position position)
        : position(position), valid(false) {};

    Token(std::unique_ptr<Morpheme> means, Position position)
        : morpheme(std::move(means)), position(position), valid(true) {};

    bool operator==(Token& other);

    bool operator!=(Token& other);

    std::string to_string() const;

    friend std::ostream& operator<<(std::ostream& os, const Token& n);
};

struct MlNum : public Morpheme
{
    MlNum() = default;

    MlNum(std::string val) : Morpheme(val, Typehints::mlnum) {};

    MlNum(int val) : Morpheme(std::to_string(val), Typehints::mlnum) {};

    static Morpheme infix(int stack, const Morpheme& op, const Morpheme& operand);

    static Morpheme infix(int stack, const Morpheme& op);

    static Token conclude(TypeGuide& guide);
};

struct MlNamespace : public Morpheme
{
    MlNamespace() = default;

    MlNamespace(std::string val) : Morpheme(val, Typehints::mlnamespace) {};

    static Token resolve(TypeGuide& guide, std::string result, Position pos);

    /**
      Continues conclusion fron partial result.
    */

    static Morpheme infix(int stack, const Morpheme& op, const Morpheme& operand);

    static Morpheme infix(int stack, const Morpheme& op);

    static Token conConclude(TypeGuide& guide, std::string result, Position pos);

    static Token conclude(TypeGuide& guide);
};

struct MlInfix : public Morpheme
{
    MlInfix() = default;

    MlInfix(std::string val, short precedence, bool isolate, bool hyphenated=false, bool unary=false, bool binary=true)
        : Morpheme(val, Typehints::mlinfix, precedence, hyphenated, isolate, unary, binary) {};
};

struct LexxedResult
{
    std::unique_ptr<BaseException> error;
    std::vector<Token> tokens;
    bool failed;

    LexxedResult(std::vector<Token>& tks)
        : tokens(std::move(tks)), failed(false) {};

    LexxedResult(std::unique_ptr<BaseException>& fault)
        : error(std::move(fault)), failed(true) {}
};


LexxedResult tokenize();

// (std::string val, short precedence, bool isolate, bool hyphenated = false, bool unary = false, bool binary = true)

const MlInfix g_lexicon_infixes[8] =
{ {"~", g_lowest_prc, false},
  {"=", g_lowest_prc, false},
  {"+", g_lowest_prc, false },
  {"-", g_lowest_prc, false, true},
  {"*", g_low_prc, false },
  {"/", g_low_prc, false},
  {"...", g_low_prc, false},
  {"return", g_low_prc, false, true} };


