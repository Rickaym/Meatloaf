#include <stdio.h>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <memory>

#include "lexer.h"
#include "task.h"
#include "errors.h"


bool Token::valid() const
{
    return this->valid_;
};

bool Token::operator==(Token &other) 
{
    return this->valid() && other.valid() && this->meaning == other.meaning;
};

bool Token::operator!=(Token &other) 
{
    return !this->operator==(other);
};

std::ostream& operator<<(std::ostream& os, const Token& n)
{
    os << n.to_string();
    return os;
}

std::string Token::to_string() const 
{
    return this->meaning.to_string() + " \\" + this->position.to_string() + "\\";
};

bool Morpheme::operator==(Morpheme &other)
{
    return this->value == other.value && this->typehint == other.typehint;
};

bool Morpheme::operator!=(Morpheme &other)
{
    return !this->operator==(other);
};

std::string Morpheme::to_string() const
{
    return this->value;
};

bool Morpheme::valid()
{
    return this->valid_;
}

Token MlNum::conclude(TypeGuide &guide) const
{
    std::string result;
    int dotCount = 0;
    Position pos = guide.capture();
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
                pos.end = guide.pos + 1;
                return Token(pos);
            }
        }
        result += guide.chr;
        guide.advance();
    };
    pos.end = guide.pos;
    return Token(MlNum(result), pos);
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
        MlInfix mph = *res;
        capture.end = guide.pos;
        return Token(mph, capture);
    }

    auto startswith = [chr](Morpheme m)
    {
        return (int)m.value.rfind(chr) != -1;
    };

    while (res == std::end(g_lexicon_infixes) && guide.eof == false && guide.is_delimiter() == false)
    {
        chr += guide.chr;
        res = is_defined_infix(chr);
        guide.advance();
    };
    capture.end = guide.pos;
    if (res != std::end(g_lexicon_infixes) && !((*res).interfix == true && (guide.is_delimiter() == false && guide.eof == false)))
    {
        MlInfix mph = *res;
        return Token(mph, capture);
    }
    else
    {
        /* Case where lexxed component is not predefined as an infix
           we pass it over to conclude a namespace */
        return MlNamespace().conConclude(guide, chr, capture);
    };
};

bool LexxedResults::failed() const
{
    return this->failed_;
}

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

        if (std::isdigit(typeGuide.chr) != 0)
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
            std::unique_ptr<BaseException> fault = std::make_unique<SyntaxError>(tk.position);
            return LexxedResults(std::move(fault));
        }
    };
    // an EOF push_back
    tokens.push_back(Token());
    return LexxedResults(tokens);
};
