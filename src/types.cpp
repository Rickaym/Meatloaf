#pragma once

#include <iostream>
#include <memory>

#include "lexer.hpp"
#include "parser.hpp"
#include "types.hpp"
#include "result.hpp"

std::string get_mltype_repr(MlType type) {
	switch (type)
	{
	case(MlType::mlint):
		return "int";
	case(MlType::mlstr):
		return "str";
	default:
		return "unk";
	}
}

std::string MlInt::repr() {
	return std::to_string(this->val);
}

Result<std::unique_ptr<MlObject>> MlInt::operate(const Token& op)
{
	if (op.lexeme.characters == "-")
	{
		return Result<std::unique_ptr<MlObject>>::success(std::make_unique<MlInt>(-this->val));
	};
}

Result<std::unique_ptr<MlObject>> MlInt::operate(std::shared_ptr<MlObject> other, const Token& op)
{
	std::shared_ptr<MlInt> int_object = std::dynamic_pointer_cast<MlInt>(other);
	if (int_object == nullptr)
	{
		return Result<std::unique_ptr<MlObject>>::failure(
			TypeError::unique_ptr(op.position, get_mltype_repr(other->type), get_mltype_repr(MlType::mlint)));
	}

	int otr = int_object->val; 
	bool success = false;
	if (op.lexeme.characters == "-")
	{
		otr = this->val - otr;
		success = true;
	}
	else if (op.lexeme.characters == "+")
	{
		otr = this->val + otr;
		success = true;
	}
	else if (op.lexeme.characters == "*")
	{
		otr = this->val * otr;
		success = true;
	}
	else if (op.lexeme.characters == "/")
	{
		otr = this->val / otr;
		success = true;
	}

	if (success)
	{
		return Result<std::unique_ptr<MlObject>>::success(MlInt::unique_ptr(otr));
	} 
	else { 
		return Result<std::unique_ptr<MlObject>>::failure(
			UnsupportedOperator::unique_ptr(op.position, op.lexeme.characters, get_mltype_repr(MlType::mlint)));
	}
}

std::string MlStr::repr() {
	return this->val;
}

Result<std::unique_ptr<MlObject>> MlStr::operate(const Token& op)
{
	return Result<std::unique_ptr<MlObject>>::failure(
		BaseException::unique_ptr(op.position, "OperationError: Affix "+op.lexeme.characters +" is not defined for strings."));
}

Result<std::unique_ptr<MlObject>> MlStr::operate(std::shared_ptr<MlObject> other, const Token& op)
{
	std::shared_ptr<MlInt> int_object = std::dynamic_pointer_cast<MlInt>(other);
	if (op.lexeme.characters == "+")
	{
		std::string appendage;
		if (int_object != nullptr)
		{
			appendage = std::to_string(int_object->val);
		}
		else {
			std::shared_ptr<MlStr> str_object = std::dynamic_pointer_cast<MlStr>(other);
			appendage = str_object->val;
		}
		return Result<std::unique_ptr<MlObject>>::success(MlStr::unique_ptr(this->val + appendage));
	}
	else if (op.lexeme.characters == "*" && int_object != nullptr)
	{
		int rep = int_object->val;
		std::string end_str;
		while (rep--) {
			end_str += this->val;
		}
		return Result<std::unique_ptr<MlObject>>::success(MlStr::unique_ptr(end_str));
	}
	return Result<std::unique_ptr<MlObject>>::failure(
		UnsupportedOperator::unique_ptr(op.position, op.lexeme.characters, get_mltype_repr(MlType::mlstr)));
}
