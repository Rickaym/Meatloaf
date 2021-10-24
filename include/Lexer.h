#pragma once

#include <vector>
#include <string>
#include <cstddef>


struct Position;

/**
 * @brief A type guide used to feed characters accurately into the lexer
 * during tokenization.
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

    TypeGuide(){};

    TypeGuide(const std::string &text) : text(text){};

    void advance();

    void retreat();

    bool chrIsDelim();

    Position capture();

private:
    void updateChar();
};

/**
 * @brief Position of a morpheme in the source code.
*/
struct Position
{
    int start = 0;
    int end = 0;
    int column = 1;
    int line = 1;

    Position() {};

    Position(int start, int end,
             int column, int line) : start(start), end(end),
                                     column(column), line(line){};

    Position(TypeGuide guide) : Position(guide.pos, guide.pos + 1, guide.column, guide.line){};

    std::string to_string();
};

/**
 * @brief A morpheme is the smallest unit of meaning characterized by semantics.
 *
 * For instance, "1" is a morpheme that represents the number one.
 * Morphemes curate it's own interactions with bound-morphemes.
 * Whatever morphemes originating from a given morpheme are called
 * "bound" morphemes.
 *
 * In this instance the negative unary operator "-" is a bound
 * morpheme that carries with it extra meaning to our prior morpheme "1".
 * Note that -1 consits of two morphemes, which can be described as a lexeme
 * that comes at play at the parsing stage.
*/
struct Morpheme
{
    std::string value = "N/A";
    std::string typehint = "udf";
    short precedence;
    bool unary;
    bool interfix;

    Morpheme() {};

    Morpheme(std::string val) : value(val) {};

    /* Some affixes are only valid when they are followed by an interfix.
       I.e. `return 0` cannot be used as `return0` since the interfix plays a role on it's validation. (the space in this example)

       This is significant if the affix name is to be used within variable names.
       The keyword `return` without this configuration of validation would mean that `return0` is looked
       at as an operation statement rather than a namespace thus restricting any namespaces hinting the affix name.
    */

    bool operator == (Morpheme &other);

    bool operator != (Morpheme &other);

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

    Token() : valid(false){};

    Token(const Morpheme &meaning) : valid(false){};

    Token(const Morpheme &meaning, const Position &position)
        : meaning(meaning), position(position), valid(true){};

    bool operator == (Token &other);

    bool operator != (Token &other);

    bool isValid();

    std::string to_string();

private:
    bool valid;
};

struct Lexer
{
    static std::vector<Token> tokenize(const std::string &text);
};
