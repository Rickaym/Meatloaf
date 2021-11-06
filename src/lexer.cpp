#include <stdio.h>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <memory>

#include "lexer.h"
#include "task.h"

std::string Source::text;

void TypeGuide::advance() 
{
    this->pos++;
    this->column++;

    if (this->newLine == true)
    {
        this->lastLineMax = this->column;
        this->line++;
        this->newLine = false;
    };
    this->update_char();
    if (this->chr == '\n')
    {
        this->newLine = true;
    };
};

Position TypeGuide::capture() 
{
    return Position(this->pos, this->pos, this->column, this->line);
};

void TypeGuide::retreat() 
{
    this->pos--;
    this->column--;

    this->update_char();
    if (this->chr == '\n') 
    {
        this->column = (int)this->lastLineMax;
        this->line--;
    };
}

void TypeGuide::update_char() 
{
    if ((unsigned)this->pos < Source::text.length()) {
        this->chr = Source::text[this->pos];
    }
    else {
        this->eof = true;
    }
};

bool TypeGuide::is_delimiter()
{
    const char *uncover = std::find(std::begin(g_lexicon_delimiters),
                                    std::end(g_lexicon_delimiters), this->chr);
    return !(uncover == std::end(g_lexicon_delimiters));
}

std::string Position::to_string()
{
    return std::to_string(this->line) + ":" + std::to_string(this->column) + "~" + std::to_string(this->end);
};

bool Token::valid() 
{
    return this->valid_;
};

bool Token::operator==(Token &other) 
{
    return this->valid() && other.valid() && this->meaning == other.meaning;
};

bool Token::operator!=(Token &other) {
    return !this->operator==(other);
};

std::string Token::to_string() {
    return "Tk<" + this->meaning.to_string() + " @ " + this->position.to_string() + ">";
};

void Morpheme::set_value(std::string val)
{
    this->value = val;
};

void Morpheme::set_typehint(std::string val)
{
    this->typehint = val;
}

void Morpheme::set_unary(bool unary)
{
    this->unary = unary;
}

void Morpheme::set_precedence(short prc)
{
    this->precedence = prc;
}

void Morpheme::set_interfix(bool interfix)
{
    this->interfix = interfix;
}

bool Morpheme::operator==(Morpheme &other)
{
    return this->value == other.value && this->typehint == other.typehint;
};

bool Morpheme::operator!=(Morpheme &other)
{
    return !this->operator==(other);
};

std::string Morpheme::to_string()
{
    return this->typehint + ":" + this->value;
};

//////////////////////////////////////////////////////////////
//               Types of Morphemes                         //
//////////////////////////////////////////////////////////////


Token MlNum::conclude(TypeGuide &guide) const
{
    std::string result;
    int dotCount = 0;
    int column = guide.column;
    int start = guide.pos;
    int line = guide.line;
    while (guide.eof != true &&
        (std::isdigit(guide.chr) || guide.chr == '.') &&
        guide.is_delimiter() == false)
    {
        if (guide.chr == '.')
        {
            if (dotCount == 0)
            {
                dotCount++;
            }
            else {
                break;
            }
        }
        result += guide.chr;
        guide.advance();
    };
    int end = guide.pos;
    return Token(MlNum(result), Position(start, end, column, line));
};


Token MlNamespace::resolve(TypeGuide &guide, std::string result, Position pos)
{
    while (guide.eof == false && guide.is_delimiter() == false)
    {
        result += guide.chr;
        guide.advance();
    };
    pos.end = guide.pos;
    return Token(MlNamespace(result), pos);
}

Token MlNamespace::conConclude(TypeGuide &guide, std::string result, Position pos) const
{
    return MlNamespace::resolve(guide, result, pos);
};

Token MlNamespace::conclude(TypeGuide &guide) const
{
    return MlNamespace::resolve(guide, "", guide.capture());
};


struct MlAffix : public Morpheme
{
};

struct MlInfix : public Morpheme
{
    MlInfix(std::string val, bool isUnary, bool interfix, short precedence)
    {
        this->set_value(val);
        this->set_typehint(__func__);
        this->set_precedence(precedence);
        this->set_unary(isUnary);
        this->set_interfix(interfix);
    };

    static Token conclude(TypeGuide &guide)
    {
        return Token();
    };
};

//////////////////////////////////////////////////////////////
//                   Naive Definition                       //
//////////////////////////////////////////////////////////////

const MlInfix g_lexicon_infixes[8] =
    {MlInfix("~", true, false, g_lowest_prc),
     MlInfix("=", true, false, g_lowest_prc),
     MlInfix("+", true, false, g_lowest_prc),
     MlInfix("-", true, false, g_lowest_prc),
     MlInfix("*", true, false, g_low_prc),
     MlInfix("/", true, false, g_low_prc),
     MlInfix("...", true, false, g_low_prc),
     MlInfix("return", true, true, g_low_prc)};

const MlInfix* is_defined_infix(std::string chr)
{
    auto isEquals = [chr](MlInfix m)
    {
        return m.value == chr;
    };
    return std::find_if(std::begin(g_lexicon_infixes), std::end(g_lexicon_infixes), isEquals);
};

/**
 * Attempts to resolve a given character from the lexicon defined in the outset.
*/
Token MlResolve(TypeGuide &guide)
{
    std::string chr(1, guide.chr);
    Position capture = guide.capture();
    guide.advance();
    const MlInfix* res = is_defined_infix(chr);
    if (res != std::end(g_lexicon_infixes))
    {
        capture.end = guide.pos;
        return Token(*res, capture);
    }

    auto startswith = [chr](Morpheme m)
    {
        return (int)m.value.rfind(chr) != -1;
    };
    // DEPRECATED
    // std::vector<Morpheme> matches;
    // std::copy_if(std::begin(g_lexicon_infixes), std::end(g_lexicon_infixes), std::back_inserter(matches), startswith);
    while (res == std::end(g_lexicon_infixes) && guide.eof == false && guide.is_delimiter() == false)
    {
        chr += guide.chr;
        res = is_defined_infix(chr);
        guide.advance();
    };
    capture.end = guide.pos;
    if (res != std::end(g_lexicon_infixes) && !((*res).interfix == true && (guide.is_delimiter() == false && guide.eof == false)))
    {
        return Token(*res, capture);
    }
    else
    {
        /* Case where lexxed component is not predefined as an infix
           we pass it over to conclude a namespace */
        return MlNamespace().conConclude(guide, chr, capture);
    };
};

//////////////////////////////////////////////////////////////
//                      Tokenizer                           //
//////////////////////////////////////////////////////////////

LexxedResults tokenize()
{
    std::vector<Token> tokens;
    TypeGuide typeGuide;

    typeGuide.advance();
    while (typeGuide.eof != true)
    {
        Token tk;
        /* We step over g_lexicon_delimiters under command of the lexer, however
           this does not mean we do the same in resolving any hinted characters. */
        if (typeGuide.is_delimiter() == true)
        {
            typeGuide.advance();
            continue;
        };

        if (std::isdigit(typeGuide.chr) == true)
        {
            tk = MlNum().conclude(typeGuide);
        }
        else
        {
            /* Certain types exists as definitions in the lexicon so we'll check for them and if not
               it is defaulted the incoming characters are defaulted to a namespace. */
            tk = MlResolve(typeGuide);

            if (tk.valid() == false)
            {
                tk = MlNamespace().conclude(typeGuide);
            };
        };
        if (tk.valid() == true)
        {
            tokens.push_back(tk);
            /* Notice that if a token is valid, it will advance the typeguide safely onto
               the appended character so we'll have no need to do it. */
        }
        else
        {
            std::unique_ptr fault = std::make_unique<SyntaxError>();
            return LexxedResults(tokens, fault);
        }
    };
    // an EOF push_back
    tokens.push_back(Token());
    return LexxedResults(tokens);
};
