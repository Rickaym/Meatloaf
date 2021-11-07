#include <vector>
#include <iostream>
#include <memory>

#include "lexer.h"
#include "parser.h"


void Task::success(std::unique_ptr<Operable>&& val)
{
    this->value = std::move(val);
}

void Task::success(std::unique_ptr<Operable>& val)
{
    this->value = std::move(val);
}

void Task::failure(std::unique_ptr<BaseException>&& fault)
{
    this->error = std::move(fault);
    this->failed = true;
}

void Task::failure(std::unique_ptr<BaseException>& fault)
{
    this->error = std::move(fault);
    this->failed = true;
}

std::ostream& operator<<(std::ostream& os, const Operable& n)
{
    os << n.to_string();
    return os;
}

void Parser::advance()
{
    this->index++;
};

Token& Parser::cur_token()
{
    return this->tokens[this->index];
};

void Parser::retreat()
{
    this->index--;
};

Morpheme Node::eval() const 
{
    // return this->token.morpheme;
    return Morpheme();
}

std::string Node::to_string() const
{
    return this->token.morpheme->to_string();
};

Morpheme BiNode::eval() const 
{
    //return this->superior->eval().infix(this->op_node.token.morpheme, inferior->eval(), 0);
    return Morpheme();
};

std::string BiNode::to_string() const 
{
    return '(' + this->superior->to_string() + ' ' + this->op_node.to_string() + ' ' + this->inferior->to_string() + ')';
};


ParsedResult Parser::ast()
{
    std::vector<std::unique_ptr<Operable>> nodes;
    Task tsk;
    this->deduce_statement(0, tsk);
    if (tsk.failed == true)
        return ParsedResult(std::move(tsk.error));
    else
        nodes.push_back(std::move(tsk.value));
    return ParsedResult(std::move(nodes));
};

void Parser::deduce_statement(int prc, Task& tsk)
{
    if (prc == (g_highest_prc + 1))
    {
        tsk.success(std::make_unique<Node>(this->cur_token()));
        return;
    };
    this->deduce_statement(prc + 1, tsk);
    if (tsk.failed == true) return;
    std::unique_ptr<Operable> superior = std::move(tsk.value);
    this->advance();
    Token op = std::move(this->cur_token());
    /* BIGGGGGGGGG PROBLEM HERE
       1>D:\Programming\C Family\Meatloaf\include\parser.h(63,5): message : see declaration of 'BiNode::BiNode'
       1>D:\Programming\C Family\Meatloaf\src\parser.cpp(105): message : see reference to function template instantiation 'std::unique_ptr<BiNode,std::default_delete<BiNode>> std::make_unique<BiNode,std::unique_ptr<Operable,std::default_delete<Operable>>,Token,std::unique_ptr<Operable,std::default_delete<Operable>>,0>(std::unique_ptr<Operable,std::default_delete<Operable>> &&,Token &&,std::unique_ptr<Operable,std::default_delete<Operable>> &&)' being compiled
       1>Done building project "Meatloaf.vcxproj" -- FAILED. */
    while (op.valid == true && op.morpheme->precedence == prc)
    {
        this->advance();
        this->deduce_statement(prc + 1, tsk);
        if (tsk.failed == true) return;
        std::unique_ptr<Operable> inferior = std::move(tsk.value);
        superior = std::make_unique<BiNode>(std::move(superior), std::move(op), std::move(inferior));
        this->advance();
        op = std::move(this->cur_token());
    };
    if (prc == 0 && this->cur_token().valid)
    {
        tsk.failure(std::make_unique<SyntaxError>(this->cur_token().position));
        return;
    }
    this->retreat();
    tsk.value = std::move(superior);
    return;
};
