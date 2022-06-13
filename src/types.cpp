#pragma once

#include <iostream>
#include <memory>

#include "lexer.hpp"
#include "types.hpp"


std::string MlInt::repr() {
    return std::to_string(this->val);
}

std::unique_ptr<MlObject> MlInt::operate(const Token& op)
{
    if (op.lexeme.characters == "-")
    {
        return std::make_unique<MlInt>(-this->val);
    };
}

std::unique_ptr<MlObject> MlInt::operate(std::shared_ptr<MlObject> other, const Token& op)
{
    std::shared_ptr<MlInt> int_object = std::shared_ptr<MlInt>(dynamic_cast<MlInt*>(other.get()));
    int otr = int_object->val;
    if (op.lexeme.characters == "-")
    {
        return std::make_unique<MlInt>(this->val-otr);
    }
    else if (op.lexeme.characters == "+")
    {
        return std::make_unique<MlInt>(this->val+otr);
    }
    else if (op.lexeme.characters == "*")
    {
        return std::make_unique<MlInt>(this->val*otr);
    }
    else if (op.lexeme.characters == "/")
    {
        return std::make_unique<MlInt>(this->val/otr);
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
