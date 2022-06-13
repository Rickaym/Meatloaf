#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

#include "lexer.hpp"
#include "types.hpp"
#include "errors.hpp"

std::string get_type_name(const LexemeDevice& tp)
{
	switch (tp)
	{
	case LexemeDevice::num:
		return "NUM";
	case LexemeDevice::symbol:
		return "SYMBOL";
	case LexemeDevice::affix:
		return "AFFIX";
	case LexemeDevice::eof:
		return "EOF";
	default:
		return "UNK";
	}
};

bool Token::operator==(Token& other)
{
	return this->valid && other.valid && this->lexeme == other.lexeme;
};

bool Token::operator!=(Token& other)
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
	{
		std::string tp = get_type_name(this->lexeme.type);
		return "... " + tp + std::string(10 - tp.size(), ' ') + this->lexeme.characters + '\n';
	}
	else
		return "[" + this->position.to_string() + "]";
};

bool Lexeme::operator==(Lexeme& other)
{
	return this->characters == other.characters && this->type == other.type;
};

bool Lexeme::operator!=(Lexeme& other)
{
	return !this->operator==(other);
};


Token mlNumConclude(TypeGuide& guide)
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
				dotCount++;
			else
			{
				pos.end = guide.pos + 1;
				return Token(pos);
			}
		}
		result += guide.chr;
		guide.advance();
	};
	pos.end = guide.pos;
	return Token(Lexeme{ result, LexemeDevice::num }, pos);
};

/**
  An optimized search of a single character with the criteria being
  if they are predefined.
*/
const Lexeme* is_defined(std::string chr)
{
	auto isEquals = [chr](Lexeme m)
	{
		return m.characters == chr;
	};
	return std::find_if(std::begin(g_lexicon_affixes), std::end(g_lexicon_affixes), isEquals);
};


Token mlSymbolResolve(TypeGuide& guide, std::string result, Position pos)
{
	while (guide.eof == false &&
		is_defined(std::string(1, guide.chr)) == std::end(g_lexicon_affixes) &&
		guide.is_delimiter() == false)
	{
		result += guide.chr;
		guide.advance();
	};
	pos.end = guide.pos;
	return Token(Lexeme{ result, LexemeDevice::symbol }, pos);
}

/**
  Concluding a symbol that is derived with the help of
  a given resultant string already
*/
Token mlSymbolConConclude(TypeGuide& guide, std::string result, Position pos)
{
	return mlSymbolResolve(guide, result, pos);
};

/**
  Concluding a symbol by itself entirely
*/
Token mlSymbolConclude(TypeGuide& guide)
{
	return mlSymbolResolve(guide, "", guide.capture());
};

/**
  Attempts to resolve a given character from
  the lexicon defined in the outset.
*/
Token mlResolve(TypeGuide& guide)
{
	std::string chr(1, guide.chr);
	Position capture = guide.capture();
	guide.advance();
	const Lexeme* res = is_defined(chr);

	if (res != std::end(g_lexicon_affixes) &&
		!(res->isolated == true &&
			(guide.is_delimiter() == false && guide.eof == false)
			))
	{
		capture.end = guide.pos;
		return Token(*res, capture);
	}

	auto startswith = [chr](Lexeme m)
	{
		return (int)m.characters.rfind(chr) != -1;
	};

	/* we must check whether if the entirity of the resultant string is defined
	   or the proceeding character is defined */
	while (res == std::end(g_lexicon_affixes) &&
		is_defined(std::string(1, guide.chr)) == std::end(g_lexicon_affixes) &&
		guide.eof == false && guide.is_delimiter() == false)
	{
		chr += guide.chr;
		guide.advance();
		res = is_defined(chr);
	};
	capture.end = guide.pos;
	/* the complete phrase has to be defined to still account as a solid infix morpheme.
	   If concluded to be true, they still have to abide by the isolation rules and so on */
	if (res != std::end(g_lexicon_affixes) && !(res->isolated == true && (guide.is_delimiter() == false && guide.eof == false)))
	{
		Lexeme mph = *res;
		return Token(mph, capture);
	}
	else
	{
		/* Case where lexxed component is not predefined as an infix
		   we pass it over to conclude a namespace */
		return mlSymbolConConclude(guide, chr, capture);
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
			tk = mlResolve(typeGuide);

			if (tk.valid == false)
			{
				tk = mlSymbolConclude(typeGuide);
			};
		};
		if (tk.valid == true)
		{
			tokens.push_back(tk);
			/* Notice that if a token is valid, it will advance the typeguide safely onto
			   the proceeding character so we'll have no need to do it. */
		}
		else
		{
			std::unique_ptr<BaseException> fault = std::make_unique<SyntaxError>(tk.position);
			return LexxedResult(fault);
		}
		endpos = tk.position;
	};
	// an EOF push_back
	tokens.push_back(Token(Lexeme{"", LexemeDevice::eof}, endpos));
	return LexxedResult(tokens);
};
