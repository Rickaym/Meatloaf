#pragma once

#include <iostream>
#include <memory>

#include "lexer.hpp"
#include "result.hpp"
#include "types.hpp"


std::string get_mltype_repr(MlType type) 
{
	switch (type) {
	case(MlType::mlint):
		return "int";
	case(MlType::mlstr):
		return "str";
	default:
		return "unk";
	}
}

MlType get_repr_mltype(std::string repr)
{
	if (repr == "int") {
		return MlType::mlint;
	}
	else if (repr == "str") {
		return MlType::mlstr;
	}
	else {
		return MlType::unknown;
	}
}

std::string MlInt::repr() 
{
	return std::to_string(this->val);
}

bool MlInt::modify(std::string modifier)
{
	if (modifier == "mb") {
		this->val *= 1000;
		return true;
	} 
	else {
		return false;
	}
}

operate_result MlInt::operate(const Token& op) 
{
	if (op.lexeme.characters == "-") {
		return operate_result::success(MlInt::shared_ptr(-this->val));
	} 
	else {
		return operate_result::failure(
			UnsupportedOperator::unique_ptr(op.position, op.lexeme.characters, get_mltype_repr(MlType::mlint)));
	}
}

operate_result MlInt::operate(std::shared_ptr<MlObject> other, const Token& op) 
{
	std::shared_ptr<MlInt> int_object = std::dynamic_pointer_cast<MlInt>(other);
	if (int_object == nullptr) {
		return operate_result::failure(
			TypeError::unique_ptr(op.position, get_mltype_repr(other->type), get_mltype_repr(MlType::mlint)));
	}

	int otr = int_object->val;
	bool success = false;
	if (op.lexeme.characters == "-") {
		otr = this->val - otr;
		success = true;
	}
	else if (op.lexeme.characters == "+") {
		otr = this->val + otr;
		success = true;
	}
	else if (op.lexeme.characters == "*") {
		otr = this->val * otr;
		success = true;
	}
	else if (op.lexeme.characters == "/") {
		otr = this->val / otr;
		success = true;
	}

	if (success) {
		return operate_result::success(MlInt::shared_ptr(otr));
	}
	else {
		return operate_result::failure(
			UnsupportedOperator::unique_ptr(op.position, op.lexeme.characters, get_mltype_repr(MlType::mlint)));
	}
}

std::string MlStr::repr() 
{
	return this->val;
}

bool MlStr::modify(std::string modifier)
{
	return false;
}

operate_result MlStr::operate(const Token& op) 
{
	return operate_result::failure(
		BaseException::unique_ptr(op.position, "OperationError: Affix " + op.lexeme.characters + " is not defined for strings."));
}

operate_result MlStr::operate(std::shared_ptr<MlObject> other, const Token& op)
{
	std::shared_ptr<MlInt> int_object = std::dynamic_pointer_cast<MlInt>(other);
	if (op.lexeme.characters == "+") {
		std::string appendage;
		if (int_object != nullptr) {
			appendage = std::to_string(int_object->val);
		}
		else {
			std::shared_ptr<MlStr> str_object = std::dynamic_pointer_cast<MlStr>(other);
			appendage = str_object->val;
		}
		return operate_result::success(MlStr::shared_ptr(this->val + appendage));
	}
	else if (op.lexeme.characters == "*" && int_object != nullptr) {
		int rep = int_object->val;
		std::string end_str;
		while (rep--) {
			end_str += this->val;
		}
		return operate_result::success(MlStr::shared_ptr(end_str));
	}
	return operate_result::failure(
		UnsupportedOperator::unique_ptr(op.position, op.lexeme.characters, get_mltype_repr(MlType::mlstr)));
}
