#include <stdio.h>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>

#include "Lexer.h"

////////////////////////////////////////////////////////////////////////////////
//    LEXOR AND LEXICAL INSTRUMENTS
////////////////////////////////////////////////////////////////////////////////

const char g_delimiters[2] = {' ', ';'};

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
    this->updateChar();
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
    std::cout << "Retreating..";
    this->pos--;
    this->column--;

    this->updateChar();
    if (this->chr == '\n')
    {
        this->column = (int)this->lastLineMax;
        this->line--;
    };
}

void TypeGuide::updateChar()
{
    if ((unsigned)this->pos < this->text.length())
    {
        this->chr = this->text[this->pos];
    }
    else
    {
        this->chr = '!';
        this->eof = true;
    }
    std::cout << this->chr << "-" << this->pos << ", ";
};

bool TypeGuide::chrIsDelim()
{
    const char *uncover = std::find(std::begin(g_delimiters),
                                    std::end(g_delimiters), this->chr);
    return !(uncover == std::end(g_delimiters));
}

Position Position::copy()
{
    return Position(this->start, this->end, this->column, this->line);
};

bool Token::isValid()
{
    return this->valid;
};

bool Token::operator==(Token &other)
{
    return this->isValid() && other.isValid() && this->meaning == other.meaning;
};

bool Token::operator!=(Token &other)
{
    return !this->operator!=(other);
};

void Morpheme::setValue(std::string val)
{
    this->value = val;
};

void Morpheme::setTypehint(std::string val)
{
    this->typehint = val;
}

bool Morpheme::operator==(Morpheme &other)
{
    return this->value == other.value && this->typehint == other.typehint;
};

bool Morpheme::operator!=(Morpheme &other)
{
    return !this->operator!=(other);
};

std::string Morpheme::toString()
{
    return this->typehint + ":" + this->value;
};

std::string Position::toString()
{
    return "ln:" + std::to_string(this->line) + " col:" + std::to_string(this->column);
};

std::string Token::toString()
{
    return "Tk<" + this->meaning.toString() + " @ " + this->position.toString() + ">";
};

//////////////////////////////////////////////////////////////
//               Types of Morphemes                         //
//////////////////////////////////////////////////////////////

struct MlNum : public Morpheme
{
    MlNum(){};

    MlNum(std::string val)
    {
        this->setValue(val);
        this->setTypehint(__func__);
    };

    MlNum(int val)
    {
        this->setValue(std::to_string(val));
        this->setTypehint(__func__);
    };

    Morpheme infix(const Morpheme &me, const Morpheme &sign, const Morpheme &operand, int stack) override
    {
        return MlNum();
    };

    Morpheme infix(const Morpheme &me, const Morpheme &sign, int stack) override
    {
        return MlNum();
    };

    static bool isNum(char key)
    {
        std::string candidates = "0123456789";
        int index = candidates.find(key);
        if (index == -1)
        {
            return false;
        }
        else
        {
            return true;
        }
    };

    static Token conclude(TypeGuide &guide)
    {
        std::cout << "Starting to resolve a number.."
                  << "\n";
        std::string result;
        //int dotCount = 0;
        int column = guide.column;
        int start = guide.pos;
        int line = guide.line;
        while (guide.eof != true && (MlNum::isNum(guide.chr) || guide.chr == '.') && guide.chrIsDelim() == false)
        {
            if (guide.chr == '.')
            {
                //dotCount++;
                break;
            }
            else
            {
                result += guide.chr;
                guide.advance();
            }
        };
        //guide.retreat();
        int end = guide.pos;
        return Token(MlNum(result), Position(start, end, column, line));
    };
};

struct MlString : public Morpheme
{
    static Token conclude(TypeGuide typeGuide) { return Token(); };
};

struct MlNamespace : public Morpheme
{
    MlNamespace(std::string val)
    {
        this->setValue(val);
        this->setTypehint(__func__);
    };

    static Token conclude(TypeGuide &guide)
    {
        std::cout << "Starting to resolve a namespace.."
                  << "\n";
        std::string result = "";
        int dotCount = 0;
        int column = guide.column;
        int start = guide.pos;
        int line = guide.line;
        while (guide.eof == false && guide.chrIsDelim() == false)
        {
            result += guide.chr;
            guide.advance();
        };
        //guide.retreat();
        int end = guide.pos;
        return Token(MlNamespace(result), Position(start, end, column, line));
    };
};

struct MlInfix : public Morpheme
{
    bool isUnary;
    bool interfix;
    short precedence;

    MlInfix(std::string val, bool isUnary_, bool interfix_, short precedence_)
        : isUnary(isUnary_), interfix(interfix_), precedence(precedence_)
    {
        this->setValue(val);
        this->setTypehint(__func__);
    };

    static Token conclude(TypeGuide &guide, std::vector<MlInfix> hints)
    {
        std::string result;
        result += guide.chr;
        while (guide.eof == false)
        {
            guide.advance();
            result += guide.chr;
        };
        return Token();
    };
};

//////////////////////////////////////////////////////////////
//                   Naive Definition                       //
//////////////////////////////////////////////////////////////

std::vector<Morpheme> g_predefs;

auto in_g_predefs(std::string chr)
{
    auto isEquals = [chr](Morpheme m)
    {
        std::cout << m.value << " VS " << chr << "\n";
        return m.value == chr;
    };
    return std::find_if(std::begin(g_predefs), std::end(g_predefs), isEquals);
};

void MlDefines(const Morpheme &type)
{
    g_predefs.push_back(type);
};

/* The lexor will resolve any namespace into a more meaningful morpheme
if it's value has been predefined */
Token MlResolve(TypeGuide &guide)
{
    std::cout << "Checking if encountering namespace is predefined to be type different\n";
    std::string chr(1, guide.chr);
    Position capture = guide.capture();
    guide.advance();
    auto res = in_g_predefs(chr);
    if (res != std::end(g_predefs))
    {
        capture.end = guide.pos;
        std::cout << "FOUND INFIX MATCH\n";
        return Token(*res, capture);
    }

    auto startswith = [chr](Morpheme m)
    {
        return m.value.rfind(chr) != -1;
    };
    std::vector<Morpheme> matches;
    std::copy_if(std::begin(g_predefs), std::end(g_predefs), std::back_inserter(matches), startswith);
    std::cout <<" EOF "<< guide.eof << "\n";
    while (res == std::end(g_predefs) && guide.eof == false)
    {
        chr += guide.chr;
        std::cout <<" EOF "<< guide.eof << "\n";
        res = in_g_predefs(chr);
        guide.advance();
    };
    capture.end = guide.pos;
    if (res != std::end(g_predefs))
    {
        return Token(*res, capture);
    }
    else
    {
        return Token(MlNamespace(chr), capture);
    };
};

short LOWEST = 0;
short LOWER = 1;
short LOW = 2;
short HIGH = 3;
short HIGHER = 4;
short HIGHEST = 5;

//////////////////////////////////////////////////////////////
//                      Tokenizer                           //
//////////////////////////////////////////////////////////////

std::vector<Token> Lexer::tokenize(const std::string &text)
{
    MlDefines(MlInfix("~", true, false, LOWEST));
    MlDefines(MlInfix("=", true, false, LOWEST));
    MlDefines(MlInfix("{{", true, false, LOWEST));
    MlDefines(MlInfix("+", true, false, LOWER));
    MlDefines(MlInfix("-", true, false, LOWER));
    MlDefines(MlInfix("*", true, false, LOW));
    MlDefines(MlInfix("/", true, false, LOW));
    MlDefines(MlInfix("...", true, false, LOW));

    std::vector<Token> tokens;
    TypeGuide typeGuide(text);

    typeGuide.advance();
    while (typeGuide.eof != true)
    {
        Token tk;
        /* We step over g_delimiters under command of the lexer, however
           this does not mean we do the same in resolving any hinted characters */
        if (typeGuide.chrIsDelim() == true)
        {
            std::cout << "Stepping over " << typeGuide.chr << "\n";
            typeGuide.advance();
            continue;
        };

        if (MlNum::isNum(typeGuide.chr) == true)
        {
            tk = MlNum::conclude(typeGuide);
        }
        else
        {
            /* Certain types are necessatitated to be pre-defined as the rudimentary toolset
               Provided they are for use of the Meatloaf compiler */
            tk = MlResolve(typeGuide);
            if (tk.isValid() == false)
            {
                tk = MlNamespace::conclude(typeGuide);
            };
        };
        if (tk.isValid() == true)
        {
            tokens.push_back(tk);
            std::cout << "Token found for " << tk.meaning.value << " stopped at ";
            std::cout << typeGuide.pos << "\n";
            //typeGuide.advance();
        }
        else
        {
            return tokens;
            std::cout << "Invalid token found for " << typeGuide.chr << "\n";
        }
    };
    Morpheme mrph;
    mrph.setTypehint("EOF");
    mrph.setValue(" ");
    Token tEof(mrph);
    tokens.push_back(tEof);

    return tokens;
};
