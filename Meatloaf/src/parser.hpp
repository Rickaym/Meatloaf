#pragma once

#include <vector>
#include <memory>
#include <ostream>

#include "types.hpp"
#include "errors.hpp"
#include "lexer.hpp"
#include "result.hpp"
#include "memory.hpp"


enum NodeType {
    node,
    binode,
    unnode
};

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
    virtual std::string to_string() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Operable& n);

    virtual Token get_superior_token () const = 0;
    
    inline NodeType get_type() {
        return this->node_type;
    };
private:
    NodeType node_type;

protected:
    Operable(NodeType node_type) : node_type(node_type) {};
};

struct Node : public Operable
{
    static inline std::unique_ptr<Node> unique_ptr(Token& tk, Lexeme& idtf) {
        return std::make_unique<Node>(tk, idtf);
    }

    static inline std::shared_ptr<Node> shared_ptr(Token& tk, Lexeme& idtf) {
        return std::make_shared<Node>(tk, idtf);
    }

    Token token;
    // positional of lexeme that constructed the node
    Lexeme identifier;

    Node(Token& tk, Lexeme& idtf) : Operable(NodeType::node), token(tk), identifier(idtf) {};

    std::string to_string() const override;

    Token get_superior_token() const override;
};

struct UnNode : public Operable
{
    static inline std::unique_ptr<UnNode> unique_ptr(Token& op, std::unique_ptr<Operable>& opnd) {
        return std::make_unique<UnNode>(op, opnd);
    }

    Token op_token;
    std::shared_ptr<Operable> operand;

    UnNode(Token& op, std::unique_ptr<Operable>& opnd) : Operable(NodeType::unnode), op_token(op), operand(std::move(opnd)) {};

    std::string to_string() const override;

    Token get_superior_token() const override;
};

struct BiNode : public Operable
{
    static inline std::unique_ptr<BiNode> unique_ptr(std::unique_ptr<Operable>& super, Token& op_token, std::unique_ptr<Operable>& infer) {
        return std::make_unique<BiNode>(super, op_token, infer);
    }

    std::shared_ptr<Operable> superior;
    Token op_token;
    std::shared_ptr<Operable> inferior;

    BiNode(std::unique_ptr<Operable>& super, Token& op_token, std::unique_ptr<Operable>& infer)
        : Operable(NodeType::binode), superior(std::move(super)), op_token(op_token), inferior(std::move(infer)) {};

    std::string to_string() const override;

    Token get_superior_token() const override;
};

struct Parser
{
    std::vector<Token> tokens;
    int index = 0;

    Token cur_token();

    Token next_token();

    void advance();

    void retreat();

    void deduce_statement(int&& prc, Result<std::unique_ptr<Operable>>& tsk, Lexeme idtf);
};

Result<std::vector<std::unique_ptr<Operable>>> ast(std::vector<Token>& tks);
