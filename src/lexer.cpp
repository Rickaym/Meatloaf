#include <stdio.h>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <memory>

#include "lexer.h"
#include "errors.h"


bool Token::operator==(Token &other) 
{
    return this->valid && other.valid && this->morpheme == other.morpheme;
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
    if (this->valid == true)
        return  this->morpheme.characters + " \\" + this->position.to_string() + "\\";
    else
        return " \\" + this->position.to_string() + "\\";
};

bool Morpheme::operator==(Morpheme &other)
{
    return this->characters == other.characters && this->typehint == other.typehint;
};

bool Morpheme::operator!=(Morpheme &other)
{
    return !this->operator==(other);
};

Token mlNumConclude(TypeGuide &guide)
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
    return Token(Morpheme{ result, Types::mlnum }, pos);
};

Token mlNamespaceResolve(TypeGuide& guide, std::string result, Position pos)
{
    while (guide.eof == false && guide.is_delimiter() == false)
    {
        result += guide.chr;
        guide.advance();
    };
    pos.end = guide.pos;
    return Token(Morpheme{ result, Types::mlnamespace }, pos);
}

Token mlNamespaceConConclude(TypeGuide &guide, std::string result, Position pos)
{
    return mlNamespaceResolve(guide, result, pos);
};

Token mlNamespaceConclude(TypeGuide &guide)
{
    return mlNamespaceResolve(guide, "", guide.capture());
};

const Morpheme* is_defined_infix(std::string chr)
{
    auto isEquals = [chr](Morpheme m)
    {
        return m.characters == chr;
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
    const Morpheme* res = is_defined_infix(chr);
    if (res != std::end(g_lexicon_infixes))
    {
        Morpheme mph = *res;
        capture.end = guide.pos;
        return Token(mph, capture);
    }

    auto startswith = [chr](Morpheme m)
    {
        return (int)m.characters.rfind(chr) != -1;
    };

    while (res == std::end(g_lexicon_infixes) && guide.eof == false && guide.is_delimiter() == false)
    {
        chr += guide.chr;
        res = is_defined_infix(chr);
        guide.advance();
    };
    capture.end = guide.pos;
    if (res != std::end(g_lexicon_infixes) && !((*res).isolated == true && (guide.is_delimiter() == false && guide.eof == false)))
    {
        Morpheme mph = *res;
        return Token(mph, capture);
    }
    else
    {
        /* Case where lexxed component is not predefined as an infix
           we pass it over to conclude a namespace */
        return mlNamespaceConConclude(guide, chr, capture);
    };
};

LexxedResult tokenize()
{
    std::vector<Token> tokens;
    TypeGuide typeGuide;

    typeGuide.advance();
    Position endpos;
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
            tk = mlNumConclude(typeGuide);
        }
        else
        {
            /* Certain types exists as definitions in the lexicon so we'll check for them and if not
               it is defaulted the incoming characters are defaulted to a namespace. */
            tk = MlResolve(typeGuide);

            if (tk.valid == false)
            {
                tk = mlNamespaceConclude(typeGuide);
            };
        };
        if (tk.valid == true)
        {
            tokens.push_back(tk);
            /* Notice that if a token is valid, it will advance the typeguide safely onto
               the appended character so we'll have no need to do it. */
        }
        else
        {
            std::unique_ptr<BaseException> fault = std::make_unique<SyntaxError>(tk.position);
            return LexxedResult(fault);
        }
        endpos = tk.position;
    };
    // an EOF push_back
    tokens.push_back(Token(endpos));
    return LexxedResult(tokens);
};
