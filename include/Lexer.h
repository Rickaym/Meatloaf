#pragma once

#include <vector>
#include <string>
#include <cstddef>

struct Position;

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

    Position copy();

    std::string toString();
};

struct Morpheme
{
    std::string value = "N/A";
    std::string typehint = "udf";

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
    virtual Morpheme infix(const Morpheme &me, const Morpheme &sign, const Morpheme &operand, int stack)
    {
        return me;
    };

    // Unary operational implementations for infixes with an absent operand.
    virtual Morpheme infix(const Morpheme &me, const Morpheme &sign, int stack)
    {
        return me;
    };

    std::string toString();

    void setValue(std::string val);

    void setTypehint(std::string val);
};

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

    std::string toString();

private:
    bool valid;
};

struct Lexer
{
    static std::vector<Token> tokenize(const std::string &text);
};
