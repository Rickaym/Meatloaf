#include <vector>
#include <iostream>
#include <memory>

#include "lexer.hpp"
#include "parser.hpp"
#include "types.hpp"


void Status::success(std::unique_ptr<Operable>&& val)
{
    this->value = std::move(val);
}

void Status::success(std::unique_ptr<Operable>& val)
{
    this->value = std::move(val);
}


void Status::failure(std::unique_ptr<BaseException>& fault)
{
    this->error = std::move(fault);
    this->failed = true;
}

void Status::failure(std::unique_ptr<BaseException>&& fault)
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

std::shared_ptr<MlObject> Node::eval() const
{
    switch (this->token.lexeme.type)
    {
    case(LexemeDevice::num):
        return MlInt::shared_ptr(this->token.lexeme.characters);
    }
    return nullptr;
}

std::shared_ptr<MlObject> BiNode::eval() const
{
    std::shared_ptr<MlObject> sup = this->superior->eval();
    return sup->operate(this->inferior->eval(), this->op_token);
}

std::shared_ptr<MlObject> UnNode::eval() const
{
    std::shared_ptr<MlObject> oprd = this->operand->eval();
    return oprd->operate(this->op_token);
}

std::string UnNode::to_string() const
{
    return "$|" + this->op_token.lexeme.characters + ' ' + this->operand->to_string() + "|$";
};


ParsedResult ast(std::vector<Token>& tks)
{
    std::vector<std::unique_ptr<Operable>> nodes;
    Status tsk;
    Parser parser{ tks };
    parser.deduce_statement(-1, tsk);
    if (tsk.failed == true)
        return ParsedResult(tsk.error);
    else
        nodes.push_back(std::move(tsk.value));
    return ParsedResult(nodes);
};

void Parser::deduce_statement(int&& prc, Status& tsk)
{
    if (prc == (g_highest_prc + 1))
    {
        // circumfixes are deduced directly as the statement that it is surrounding, therefore 
        // they must be stepped over, with no significance but in precedence overriding
        if (this->cur_token().lexeme.positional == LexemePositional::circumfix) 
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
            if (op.lexeme.positional == LexemePositional::prefix)
                tsk.success(Node::unique_ptr(this->cur_token()));
            else
                this->deduce_statement(0, tsk);

            tsk.success(UnNode::unique_ptr(op, tsk.value));
        }
        else 
            tsk.success(Node::unique_ptr(this->cur_token()));
        return;
    };
    this->deduce_statement(prc + 1, tsk);
    
    if (tsk.failed)
        return;

    std::unique_ptr<Operable> superior = std::move(tsk.value);
    this->advance();

    Token op = this->cur_token();
    while (op.valid && op.lexeme.precedence == prc && op.lexeme.binary())
    {
        this->advance();

        if (this->cur_token().lexeme.type == LexemeDevice::eof)
            return tsk.failure(SyntaxError::unique_ptr(op.position, "Unexpected EOF while parsing"));
        
        this->deduce_statement(prc + 1, tsk);
        if (tsk.failed)
            return;
        superior = BiNode::unique_ptr(superior, op, tsk.value);

        this->advance();
        op = this->cur_token();
    };
    // having not reached the end of tokens in the end of
    // the initiation call is indicative of an invalid syntax
    if (prc == -1 && this->index != this->tokens.size()-1)
        return tsk.failure(SyntaxError::unique_ptr(op.position));

    // compensate an imminent overstep in parsing
    this->retreat();
    return tsk.success(superior);
};