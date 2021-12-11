#include <vector>
#include <iostream>
#include <memory>

#include "lexer.hpp"
#include "parser.hpp"
#include "types.hpp"


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
        this->index++;
};

Token Parser::cur_token()
{
    return this->tokens[this->index];
}

void Parser::retreat()
{   
    if (this->index > 0)
        this->index--;
};

std::string Node::to_string() const
{
    return this->token.lexeme.characters;
};

std::string BiNode::to_string() const 
{
    return "$(" + this->superior->to_string() + ' ' + this->op_token.lexeme.characters + ' ' + this->inferior->to_string() + ")$";
};

std::unique_ptr<MlObject> Node::eval() const
{
    switch (this->token.lexeme.typehint)
    {
    case(MlTypes::mlnum):
        return std::make_unique<MlInt>(this->token.lexeme.characters);
    }
    return nullptr;
}

std::unique_ptr<MlObject> BiNode::eval() const
{
    std::unique_ptr<MlObject> sup = std::move(this->superior->eval());
    std::unique_ptr<MlObject> inf = std::move(this->inferior->eval());
    sup->operate(inf, this->op_token);
}

std::unique_ptr<MlObject> UnNode::eval() const
{
    std::unique_ptr<MlObject> oprd = std::move(this->operand->eval());
    oprd->operate(this->op_token);
}

std::string UnNode::to_string() const
{
    return "$|" + this->op_token.lexeme.characters + ' ' + this->operand->to_string() + "|$";
};


ParsedResult ast(std::vector<Token>& tks)
{
    std::vector<std::unique_ptr<Operable>> nodes;
    Task tsk;
    Parser p{ tks };
    p.deduce_statement(-1, tsk);
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
        // circumfixes are deduced directly as the statement that it is surrounding, therefore 
        // they must be stepped over, with no significance but in precedence overriding
        if (this->cur_token().lexeme.typehint == MlTypes::mlcircumfix) 
        {
            this->advance();
            this->deduce_statement(0, tsk);
            this->advance();
        }
        else if (this->cur_token().lexeme.unary == true)
        {
            Token op = this->cur_token();
            /* prefixes only consumes a single proceeding operand whereas 
               any non-prefix affixes that responds to unary calls consume
               the entire following statement */
            this->advance();
            if (op.lexeme.typehint == MlTypes::mlprefix)
                tsk.success(std::make_unique<Node>(this->cur_token()));
            else
                this->deduce_statement(0, tsk);

            tsk.success(std::make_unique<UnNode>(op, tsk.value));
        }
        else 
            tsk.success(std::make_unique<Node>(this->cur_token()));
        return;
    };
    this->deduce_statement(prc + 1, tsk);
    if (tsk.failed)
        return;
    std::unique_ptr<Operable> superior = std::move(tsk.value);
    this->advance();
    Token op = this->cur_token();
    while (op.valid && op.lexeme.precedence == prc && op.lexeme.binary)
    {
        this->advance();
        if (this->cur_token().lexeme.typehint == MlTypes::mleof)
            return tsk.failure(std::make_unique<SyntaxError>(op.position, "Unexpected EOF while parsing"));
        this->deduce_statement(prc + 1, tsk);
        if (tsk.failed)
            return;
        superior = std::make_unique<BiNode>(superior, op, tsk.value);

        this->advance();
        op = this->cur_token();
    };
    // having not reached the end of tokens in the end of
    // the initiation call is indicative of an invalid syntax
    if (prc == -1 && this->index != this->tokens.size()-1)
        return tsk.failure(std::make_unique<SyntaxError>(op.position));

    // compensate an imminent overstep in parsing
    this->retreat();
    return tsk.success(superior);
};