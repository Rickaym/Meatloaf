#pragma once

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <cstddef>

#include "errors.hpp"
#include "guide.hpp"

/**
 Precedence (prc) constants.
*/
const short g_lowest_prc = 0;
const short g_low_prc = 1;
const short g_high_prc = 2;
const short g_highest_prc = 3;

const char g_lexicon_delimiters[2] = {' ', ';'};

/**
 A position identifier for a lexeme, i.e., whether if the lexeme occur before a 
 device (prefix), after a device (suffix), between devices (infix) and around
 device(s) (circumfix).
 
 Members
 -------
 prefix    : occur before a device
 suffix    : occur after a device
 circumfix : occur around device(s)
 infix     : occur between devices
*/
enum class LexemePositional
{
    prefix,
    infix,
    circumfix,
    unknown
};

/**
 A utility identifier for a lexeme, i.e., its utility for being included in 
 the source code. Some devices are numerical (used as numbers), symbolic 
 (used as symbols) or affixal (used as a special kind of modifier-symbol).
 Here is where lexeme finitude or infinitude is determined. A numeric device 
 is an infinite lexeme as there exists infinite an form of lexemes that ascribes
 to this utility, whereas there is a finite amount (infact only one) of lexemes that can 
 be identified as an eof.
 
 Members
 -------
 num    : an infinite lexeme, numeric type, every numeric string of characters
 symbol : an infinite lexeme, symbol type, every literal string of characters until it is resolved
               into it's real type
 affix  : an infinite lexeme, affix type, modifies/annotates any other devices or itself
 eof    : a finite lexeme, EOF type, remarks the end of a token stream.
*/
enum class LexemeDevice
{
    num,
    symbol,
    affix,
    unknown,
    eof
};

/**
 A lexeme is a word or a phrase in the source code that has positional
 and utility significance. 
 
 The unary flag only matters if the lexeme is an infix but still wants to
 respond to unary calls as specifically prefixes. Infixes that want to respond
 to unary calls as suffixes should additionally define a separate suffix.
 
 characters : the string representation of the lexeme
 type   : the device identifier
 positional : the position identifier
 isolated   : whether if the lexeme needs to be surrounded by delimiters
 unary      : whether if the lexeme responds to unary calls as an operator
 binary     : whether if the lexeme responds to binary calls as an operator
*/
struct Lexeme
{
    std::string characters;
    LexemeDevice type = LexemeDevice::unknown;
    LexemePositional positional = LexemePositional::unknown;
    short precedence = g_lowest_prc;
    
    bool isolated = false;
    bool unary = false;

    bool binary() {
        return positional == LexemePositional::infix;
    };

    bool operator==(Lexeme &other);

    bool operator!=(Lexeme &other);
};

/**
 A Token is a simple container with a meaning (lexeme) and it's position in
 the source code.
*/
struct Token
{
    Lexeme lexeme;
    Position position;
    bool valid;

    Token() : valid(false) {};

    Token(Position position)
        : position(position), valid(false) {};

    Token(Lexeme means, Position pos)
        : lexeme(means), position(pos), valid(true) {};

    bool operator==(Token& other);

    bool operator!=(Token& other);

    std::string to_string() const;

    friend std::ostream& operator<<(std::ostream& os, const Token& n);
};


struct LexxedResult
{
    std::unique_ptr<BaseException> error;
    std::vector<Token> tokens;
    bool failed;

    LexxedResult(std::vector<Token>& tks)
        : tokens(tks), failed(false) {};

    LexxedResult(std::unique_ptr<BaseException>& fault)
        : error(std::move(fault)), failed(true) {}
};

LexxedResult tokenize();

std::string get_device_repr(const LexemeDevice& tp);

const Lexeme g_lexicon_affixes[21] =
{ {"...", LexemeDevice::affix, LexemePositional::unknown, g_low_prc, false, false},
  {"mstr", LexemeDevice::affix, LexemePositional::unknown, g_low_prc, true, true},
  {"~", LexemeDevice::affix, LexemePositional::prefix, g_lowest_prc, false, true},
  {"return", LexemeDevice::affix, LexemePositional::prefix, g_low_prc, true, true},
  {"str", LexemeDevice::affix, LexemePositional::prefix, g_low_prc, true, true},
  {"int", LexemeDevice::affix, LexemePositional::prefix, g_low_prc, true, true},
  {"float", LexemeDevice::affix, LexemePositional::prefix, g_low_prc, true, true},
  {"if", LexemeDevice::affix, LexemePositional::prefix, g_low_prc, true, true},
  {":", LexemeDevice::affix, LexemePositional::infix, g_lowest_prc, false, false},
  {",", LexemeDevice::affix, LexemePositional::infix, g_lowest_prc, false, false},
  {"=", LexemeDevice::affix, LexemePositional::infix, g_lowest_prc, false, false},
  {"+", LexemeDevice::affix, LexemePositional::infix, g_lowest_prc, false, false},
  {".", LexemeDevice::affix, LexemePositional::infix, g_lowest_prc, false, false},
  {"-", LexemeDevice::affix, LexemePositional::infix, g_lowest_prc, false, true},
  {"*", LexemeDevice::affix, LexemePositional::infix, g_low_prc, false, false},
  {"/", LexemeDevice::affix, LexemePositional::infix, g_low_prc, false, false},
  {"\"", LexemeDevice::affix, LexemePositional::circumfix, g_low_prc, false, false},
  {"{", LexemeDevice::affix, LexemePositional::circumfix, g_lowest_prc, false, false},
  {"}", LexemeDevice::affix, LexemePositional::circumfix, g_lowest_prc, false, false},
  {"(", LexemeDevice::affix, LexemePositional::circumfix, g_lowest_prc, false, false},
  {")", LexemeDevice::affix, LexemePositional::circumfix, g_lowest_prc, false, false}
 };