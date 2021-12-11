#pragma once

#include "types.hpp"

std::unique_ptr<MlObject> MlObject::getPrivates(std::string attrname)
{
};

std::unique_ptr<MlObject> MlInt::operate(const std::unique_ptr<MlObject>& other, const Token& op)
{
    /* stoi() for interger
       stod() for double
       stof() for float */
    if (op.lexeme.characters == "-")
    {
        return std::make_unique<MlInt>(this->val-other->getMetaValue());
    }
    else if (op.characters == "+")
    {
        return std::make_unique<MlInt>(std::stoi(thisNum.characters) + std::stoi(operand.characters));
    }
    else if (op.characters == "*")
    {
        return std::make_unique<MlInt>(std::stoi(thisNum.characters) * std::stoi(operand.characters));
    }
    else if (op.characters == "/")
    {
        return std::make_unique<MlInt>(std::stoi(thisNum.characters) / std::stoi(operand.characters));
    };
    return nullptr;
};

std::unique_ptr<MlObject> mlNumInfix(const Lexeme& thisNum, const Lexeme& op)
{
    if (op.characters == "-")
    {
        return std::make_unique<MlInt>(0 - std::stoi(thisNum.characters));
    }
    return nullptr;
};
