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
    std::string value;
    std::string typehint;
    short precedence;
    bool unary;
    bool interfix;

    Morpheme() = default;

    bool operator==(Morpheme &other);

    bool operator!=(Morpheme &other);

    // Binary Operational implementations for affixes.
    virtual Morpheme infix(const Morpheme& sign, const Morpheme& operand, int stack) { return Morpheme(); };

    // Unary operational implementations for infixes with an absent operand.
    virtual Morpheme infix(const Morpheme& sign, int stack) { return Morpheme(); };

    std::string to_string() const;

    bool valid();;

private:
    bool valid_;

protected:
    Morpheme(std::string val, std::string tph)
        : value(val), typehint(tph), valid_(true) {};

    Morpheme(std::string val, std::string tph, bool isUnary, bool itfx, short prc)
        : value(val), typehint(tph), precedence(prc), interfix(itfx), unary(isUnary) {};

};

/**
  A Token is a simple container with a meaning (morpheme) and it's position in
  the source code.
*/
struct Token
{
    Morpheme meaning;
    Position position;

    Token() : valid_(false) {};

    Token(Position position)
        : position(position), valid_(false) {};

    Token(Morpheme meaning, Position position)
        : meaning(meaning), position(position), valid_(true) {};

    bool operator==(Token& other);

    bool operator!=(Token& other);

    bool valid() const;

    std::string to_string() const;

    friend std::ostream& operator<<(std::ostream& os, const Token& n);

private:
    bool valid_;
};

struct LexxedResults
{
    std::unique_ptr<BaseException> error;
    std::vector<Token> tokens;
    
    LexxedResults(std::vector<Token> tks) 
        : tokens(tks), error(std::make_unique<BaseException>()), failed_(false) {};

    LexxedResults(std::unique_ptr<BaseException> fault)
        : error(std::move(fault)), failed_(true) {};

    bool failed() const;

private:
    bool failed_;
}; 

struct MlNum : public Morpheme
{
    MlNum() = default;

    MlNum(std::string val) : Morpheme(val, __func__) {};

    MlNum(int val) : Morpheme(std::to_string(val), __func__) {};

    /* TO RESOLVE: 
    
    Morpheme infix(const Morpheme& op, const Morpheme& operand, int stack);

    Morpheme infix(const Morpheme& op, int stack) override; */

    Token conclude(TypeGuide& guide) const;
};

struct MlNamespace : public Morpheme
{
    MlNamespace() = default;

    MlNamespace(std::string val) : Morpheme(val, __func__) {};

    static Token resolve(TypeGuide& guide, std::string result, Position pos);

    /**
      Continues conclusion fron partial result.
    */
    Token conConclude(TypeGuide& guide, std::string result, Position pos) const;

    Token conclude(TypeGuide& guide) const;
};

struct MlInfix : public Morpheme
{
    MlInfix() = default;

    MlInfix(std::string val, bool isUnary, bool interfix, short precedence)
        : Morpheme(val, __func__, isUnary, interfix,precedence) {};
};


LexxedResults tokenize();


