#pragma once

#include <vector>
#include <string>
#include <cstddef>

const short LOWEST = 0;
const short LOWER = 1;
const short LOW = 2;
const short HIGH = 3;
const short HIGHER = 4;
const short HIGHEST = 5;

const char g_lexicon_delimiters[2] = {' ', ';'};

struct Position;

/**
 * @brief A type guide used to feed characters into the lexer
 * during tokenization, this isn't part of the lexer class as opposed to
 * the parser having it's own internal guide.
*/
struct TypeGuide
{
    const std::string text;
    int pos = -1;
    char chr = '+';
    int len = 0;
    int column = 0;
    int line = 1;
    int lastLineMax = 0;
    bool newLine = false;
    bool eof = false;

    TypeGuide(const std::string& text) : text(text) {};

    void advance();

    void retreat();

    bool chrIsDelim();

    Position capture();

private:
    void updateChar();
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

    Morpheme(std::string val="N/A") : value(val) {};

    bool operator==(Morpheme &other);

    bool operator!=(Morpheme &other);

    // Binary Operational implementations for affixes.
    virtual Morpheme infix(const Morpheme &sign, const Morpheme &operand, int stack)
    {
        return *this;
    };

    // Unary operational implementations for infixes with an absent operand.
    virtual Morpheme infix(const Morpheme &sign, int stack)
    {
        return *this;
    };

    std::string to_string();

protected:
    void setTypehint(std::string val);
    void setPrecedence(short prc);
    void setUnary(bool unary);
    void setInterfix(bool intfx);
    void setValue(std::string val);
};

/**
 * A Token is a simple container with a meaning (morpheme) and it's position in
 * the source code.
*/
struct Token
{
    Morpheme meaning;
    Position position;

    Token() : valid(false) {};

    Token(const Morpheme &meaning) : valid(false){};

    Token(const Morpheme &meaning, const Position &position)
        : meaning(meaning), position(position), valid(true){};

    bool operator==(Token &other);

    bool operator!=(Token &other);

    bool isValid();

    std::string to_string();

private:
    bool valid;
};

struct Lexer
{
    static std::vector<Token> tokenize(const std::string &text);
};
