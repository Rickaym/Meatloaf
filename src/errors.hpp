#pragma once

#include <string>
#include <sstream>

#include "guide.hpp"

/**
  ---------- Errors ------------
  Runtime and compile time errors.
*/
struct BaseException 
{
	BaseException() = default;

	BaseException(std::string brf, Position& pos) : brief_(brf), position_(pos) {};

	std::string traceback() const;

	friend std::ostream& operator<<(std::ostream& os, const BaseException& n);

private:
	std::string brief_;
	Position position_;	
};


struct SyntaxError : public BaseException
{
	SyntaxError(Position& pos, std::string brief="Unexpected proceeding character or token") 
		: BaseException("SyntaxError: "+brief, pos) {};
};


struct UnknownException;

struct RuntimeException;
