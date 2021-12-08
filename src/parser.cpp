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


void Task::failure(std::unique_ptr<BaseException>& fault)
{
    this->error = std::move(fault);
    this->failed = true;
}

void Task::failure(std::unique_ptr<BaseException>&& fault)
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
    if (this->index < (this->tokens.size()-1))
    {
        this->index++;
    }
};

Token Parser::cur_token()
{
    return this->tokens[this->index];
}

void Parser::retreat()
{   
    if (this->index > 0)
    {
        this->index--;
    }
};

Morpheme Node::eval() const 
{
    // return this->token.morpheme;
    return Morpheme();
}

std::string Node::to_string() const
{
    return this->token.morpheme.characters;
};

Morpheme BiNode::eval() const 
{
    //return this->superior->eval().infix(this->op_node.token.morpheme, inferior->eval(), 0);
    return Morpheme();
};

std::string BiNode::to_string() const 
{
    return '(' + this->superior->to_string() + ' ' + this->op_token.morpheme.characters + ' ' + this->inferior->to_string() + ')';
};


ParsedResult Parser::ast()
{
    std::vector<std::unique_ptr<Operable>> nodes;
    Task tsk;
    this->deduce_statement(0, tsk);
    if (tsk.failed == true)
        return ParsedResult(tsk.error);
    else
        nodes.push_back(std::move(tsk.value));
    return ParsedResult(nodes);
};

void Parser::deduce_statement(int&& prc, Task& tsk)
{
    if (prc == (g_highest_prc + 1))
    {
        tsk.success(std::make_unique<Node>(this->cur_token()));
        return;
    };
    this->deduce_statement(prc + 1, tsk);
    if (tsk.failed == true)
    {
        return;
    }
    std::unique_ptr<Operable> superior = std::move(tsk.value);
    this->advance();
    Token op = this->cur_token();
    while (op.valid == true && op.morpheme.precedence == prc)
    {
        this->advance();
        // deduced a new statement as for the inferior part
        // of the binary node
        this->deduce_statement(prc + 1, tsk);
        if (tsk.failed == true)
        {
            return;
        }

        superior = std::make_unique<BiNode>(superior, op, tsk.value);
        this->advance();
        op = this->cur_token();
    };
    // to compensate an imminent overstep
    this->retreat();
    tsk.success(superior);
    return;
};
