#pragma once

#include <vector>
#include <memory>
#include <ostream>

#include "lexer.hpp"
#include "errors.hpp"
#include "types.hpp"

/**
 A lexeme a singular or a combination of morphemes, lexemes that look-a-like does not change
 meaning. 
 For instance -1  is a lexeme that represents a negative number one.
 1+2 is a lexeme that represents a positive number one being added to a positive number 2.
 
 1 alone can also be a lexeme in some situations. All lexemes has (or are) morphemes but not all
 morphemes can be lexemes.
*/

struct Operable
{
    std::shared_ptr<MlObject> virtual eval() const = 0;
    
    std::string virtual to_string() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Operable& n);
};

struct Node : public Operable
{
    static inline std::unique_ptr<Node> unique_ptr(Token& tk) {
        return std::make_unique<Node>(tk);
    }

    Token token;

    Node(Token& tk) : token(tk) {};

    std::shared_ptr<MlObject> eval() const override;

    std::string to_string() const override;
};

struct UnNode : public Operable
{
    static inline std::unique_ptr<UnNode> unique_ptr(Token& op, std::unique_ptr<Operable>& opnd) {
        return std::make_unique<UnNode>(op, opnd);
    }

    Token op_token;
    std::unique_ptr<Operable> operand;

    UnNode(Token& op, std::unique_ptr<Operable>& opnd) : op_token(op), operand(std::move(opnd)) {};

    std::shared_ptr<MlObject> eval() const override;

    std::string to_string() const override;
};

struct BiNode : public Operable
{
    static inline std::unique_ptr<BiNode> unique_ptr(std::unique_ptr<Operable>& super, Token& op_token, std::unique_ptr<Operable>& infer) {
        return std::make_unique<BiNode>(super, op_token, infer);
    }

    std::unique_ptr<Operable> superior;
    Token op_token;
    std::unique_ptr<Operable> inferior;

    BiNode(std::unique_ptr<Operable>& super, Token& op_token, std::unique_ptr<Operable>& infer)
        : superior(std::move(super)), op_token(op_token), inferior(std::move(infer)) {};

    std::shared_ptr<MlObject> eval() const override;

    std::string to_string() const override;
};  

struct Status
{
    bool failed = false;
    std::unique_ptr<Operable> value;
    std::unique_ptr<BaseException> error;

    void success(std::unique_ptr<Operable>&& val);
       
    void success(std::unique_ptr<Operable>& val);

    void failure(std::unique_ptr<BaseException>&& val);

    void failure(std::unique_ptr<BaseException>& fault);
};

struct ParsedResult
{
    bool failed;
    std::vector<std::unique_ptr<Operable>> nodes;
    std::unique_ptr<BaseException> error;

    ParsedResult(std::vector<std::unique_ptr<Operable>>& nds)
        : nodes(std::move(nds)), failed(false) {};

    ParsedResult(std::unique_ptr<BaseException>& fault)
        : error(std::move(fault)), failed(true) {};

};

struct Parser
{
    std::vector<Token> tokens;
    int index = 0;

    Token cur_token();

    void advance();

    void retreat();

    void deduce_statement(int&& prc, Status& tsk);
};

ParsedResult ast(std::vector<Token>& tks);
