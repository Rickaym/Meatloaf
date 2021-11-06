#pragma once

#include <vector>
#include <string>
#include <cstddef>

#include "errors.h"

/**
 Precedence (prc) constants.
*/
const short g_lowest_prc = 0;
const short g_low_prc = 1;
const short g_high_prc = 2;
const short g_highest_prc = 3;

const char g_lexicon_delimiters[2] = {' ', ';'};



/**
 The static source object shared by the different parts of the compiler.
*/
struct Source
{
    static std::string text;
};

/**
  A type guide used to feed characters into the lexer
  during tokenization, this isn't part of the lexer class as opposed to
  the parser having it's own internal guide.
*/
struct TypeGuide
{
    int pos = -1;
    char chr = '.';
    int len = 0;
    int column = 0;
    int line = 1;
    int lastLineMax = 0;
    bool newLine = false;
    bool eof = false;

    TypeGuide() = default;

    void advance();

    void retreat();

    bool is_delimiter();

    Position capture();

private:
    void update_char();
};

/**
 * @brief A description of the position a morpheme has in the source code.
*/
struct Position
{
    int start = 0;
    int end = 0;
    int column = 1;
    int line = 1;

    Position() = default;

    Position(int start, int end, int column, int line) : start(start), end(end),
        column(column), line(line) {};

    Position(TypeGuide guide) : Position(guide.pos, guide.pos + 1, guide.column, guide.line) {};

    std::string to_string();
};

/**
 * @brief A morpheme is the smallest unit of meaning characterized by semantics.
 *
 * For instance, "1" is a morpheme that represents the number one.
 * Morphemes curate it's own interactions with bound-morphemes.
 * New morphemes are created from combining a given morpheme and
 * "bound" morphemes whereas "bound" morphemes are invalid alone.
 *
 * In this instance the negative unary operator "-" is a bound
 * morpheme that carries with it extra meaning to our prior morpheme "1".
 * Note that -1 consits of two morphemes, a lexeme.
 * 
 * Some affixes are only valid when they are followed by an interfix.
 * I.e. `return 0` cannot be used as `return0` since the interfix character, a whitespace
 * plays a role on it's validation. 
 * This is significant if the affix name is allowed to be used within variable names.
 * The keyword `return` without this configuration of validation would mean that `return0` is looked
 * at as an operation statement rather than a namespace thus restricting any namespaces hinting the affix name
*/
struct Morpheme
{
    std::string value;
    std::string typehint;
    short precedence;
    bool unary;
    bool interfix;

    Morpheme() = default;

    Morpheme(Morpheme& v) {};

    Morpheme(std::string val) : valid_(false) {};
    
    Morpheme(std::string val) : value(val), valid_(true) {};

    Morpheme(std::string val, std::string tph, bool isUnary, bool itfx, short prc)
        : value(val), typehint(tph), precedence(prc), interfix(itfx), unary(isUnary) {};

    bool operator==(Morpheme &other);

    bool operator!=(Morpheme &other);

    // Binary Operational implementations for affixes.
    virtual Morpheme infix(const Morpheme& sign, const Morpheme& operand, int stack) { return Morpheme(); };

    // Unary operational implementations for infixes with an absent operand.
    virtual Morpheme infix(const Morpheme& sign, int stack) { return Morpheme(); };

    std::string to_string();

    bool valid();;

private:
    bool valid_;

protected:
    void set_typehint(std::string val);
    void set_precedence(short prc);
    void set_unary(bool unary);
    void set_interfix(bool intfx);
    void set_value(std::string val);
};

/**
 * A Token is a simple container with a meaning (morpheme) and it's position in
 * the source code.
*/
struct Token
{
    Morpheme meaning;
    Position position;

    Token() : valid_(false) {};

    Token(Morpheme meaning) : valid_(false) {};
     
    Token(Morpheme meaning, Position position)
        : meaning(meaning), position(position), valid_(true) {};

    bool operator==(Token& other);

    bool operator!=(Token& other);

    bool valid();

    std::string to_string();

private:
    bool valid_;
};

struct LexxedResults
{
    std::unique_ptr<BaseException>& error;
    std::vector<Token> tokens;
    
    LexxedResults(std::vector<Token> tks) 
        : tokens(tks), error(std::make_unique<BaseException>()), failed_(false) {};

    LexxedResults(std::vector<Token> tks, std::unique_ptr<BaseException>& fault) 
        : tokens(tks), error(fault), failed_(true) {};

    bool failed();

private:
    bool failed_;
}; 

struct MlNum : public Morpheme
{
    MlNum() = default;

    MlNum(std::string val)
    {
        this->set_value(val);
        this->set_typehint(__func__);
    };

    MlNum(int val)
    {
        this->set_value(std::to_string(val));
        this->set_typehint(__func__);
    };

    Morpheme infix(const Morpheme& op, const Morpheme& operand, int stack);

    Morpheme infix(const Morpheme& op, int stack) override;

    Token conclude(TypeGuide& guide) const;
};

struct MlNamespace : public Morpheme
{
    MlNamespace() = default;

    MlNamespace(std::string val)
    {
        this->set_value(val);
        this->set_typehint(__func__);
    };

    static Token resolve(TypeGuide& guide, std::string result, Position pos);

    /**
      Continues conclusion fron partial result.
    */
    Token conConclude(TypeGuide& guide, std::string result, Position pos) const;

    Token conclude(TypeGuide& guide) const;
};

struct MlInfix : public Morpheme
{
    MlInfix(std::string val, bool isUnary, bool interfix, short precedence)
        : Morpheme(val, __func__, isUnary, interfix,precedence) {};
};


LexxedResults tokenize();


