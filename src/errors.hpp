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
	static inline std::unique_ptr<BaseException> unique_ptr(Position pos, std::string brief) {
		return std::make_unique<BaseException>(pos, brief);
	}

	BaseException() = default;

	BaseException(Position& pos, std::string brf) : brief_(brf), position_(pos) {};

	std::string traceback() const;

	friend std::ostream& operator<<(std::ostream& os, const BaseException& n);

private:
	std::string brief_;
	Position position_;	
};


struct SyntaxError : public BaseException
{
	static inline std::unique_ptr<SyntaxError> unique_ptr(Position pos, std::string brief = "Unexpected proceeding character or token") {
		return std::make_unique<SyntaxError>(pos, brief);
	}

	SyntaxError(Position& pos, std::string brief)
		: BaseException(pos, "SyntaxError: " + brief) {};
};


struct TypeError : public BaseException
{
	static inline std::unique_ptr<TypeError> unique_ptr(Position pos, std::string wrong_type, std::string suppose_type) {
		return std::make_unique<TypeError>(pos, wrong_type, suppose_type);
	}

	TypeError(Position & pos, std::string wrong_type, std::string suppose_type)
		: BaseException(pos, "TypeError: '" + wrong_type + "' is supposed to be '" + suppose_type + "'.") {};
};

struct UnsupportedOperator: public BaseException
{
	static inline std::unique_ptr<UnsupportedOperator> unique_ptr(Position pos, std::string op, std::string type) {
		return std::make_unique<UnsupportedOperator>(pos, op, type);
	}

	UnsupportedOperator(Position& pos, std::string op, std::string type)
		: BaseException(pos, "UnsupportedOoperator: " + op + " for type " + type + " is undefined.") {};
};


struct RuntimeException;
