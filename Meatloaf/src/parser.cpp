#include <vector>
#include <iostream>
#include <memory>

#include "lexer.hpp"
#include "parser.hpp"
#include "result.hpp"


std::ostream& operator<<(std::ostream& os, const Operable& n)
{
	os << n.to_string();
	return os;
}

void Parser::advance()
{
	if (this->index < (this->tokens.size() - 1)) {
		this->index++;
	}
}

Token Parser::cur_token()
{
	return this->tokens[this->index];
}

Token Parser::next_token()
{
	if (this->index < (this->tokens.size()-2)) {
		return this->tokens[this->index+1];
	} 
	else {
		return this->tokens[this->index];
	}
}

void Parser::retreat()
{
	if (this->index > 0) {
		this->index--;
	}
}

std::string UnNode::to_string() const
{
	return "$|" + this->op_token.lexeme.characters + ' ' + this->operand->to_string() + "|$";
};

Token UnNode::get_superior_token() const {
	return this->operand->get_superior_token();
}

std::string Node::to_string() const
{
	return this->token.lexeme.characters;
}

Token Node::get_superior_token() const {
	return this->token;
}

std::string BiNode::to_string() const
{
	return "$(" + this->superior->to_string() + ' ' + this->op_token.lexeme.characters + ' ' + this->inferior->to_string() + ")$";
};

Token BiNode::get_superior_token() const {
	return this->superior->get_superior_token();
}

Result<std::vector<std::unique_ptr<Operable>>> ast(std::vector<Token>& tks)
{
	std::vector<std::unique_ptr<Operable>> nodes;
	auto tsk = Result<std::unique_ptr<Operable>>::failure(
		BaseException::unique_ptr(tks[0].position, "BaseException: Parser unsuccessful")
	);
	Parser parser{ tks };

	// an invalid initiating lexeme
	Lexeme init;
	parser.deduce_statement(-1, tsk, init);
	if (tsk.failed == true) {
		return Result<std::vector<std::unique_ptr<Operable>>>::failure(tsk.error);
	}
	else {
		nodes.push_back(std::move(tsk.value));
	}
	return Result<std::vector<std::unique_ptr<Operable>>>::success(nodes);
};

void Parser::deduce_statement(int&& prc, Result<std::unique_ptr<Operable>>& tsk, Lexeme idtf)
{
	if (prc == (g_highest_prc + 1)) {
		// circumfixes are deduced directly as the statement that it is surrounding, therefore 
		// they must be stepped over, with no significance but in precedence overriding
		if (this->cur_token().lexeme.positional == LexemePositional::circumfix) {
			Lexeme tmp = this->cur_token().lexeme;
			this->advance();
			this->deduce_statement(0, tsk, tmp);
			this->advance();
		}
		else if (this->cur_token().lexeme.positional == LexemePositional::suffix) {
			return tsk.as_failure(
				RuntimeError::unique_ptr(this->cur_token().position, "Isolated suffix missing a parent"));
		}
		else if (this->cur_token().lexeme.unary == true || this->cur_token().lexeme.positional == LexemePositional::prefix) {
			Token prefix = this->cur_token();
			/* prefixes only consumes a single proceeding operand whereas
			   any non-prefix affixes that responds to unary calls consume
			   the entire following statement */
			this->advance();
			this->deduce_statement(0, tsk, idtf);
			tsk.as_success(UnNode::unique_ptr(prefix, tsk.value));
		}
		else { 
			Token tk = this->cur_token();;
			if (this->next_token().lexeme.positional == LexemePositional::suffix) {
				this->advance();
				idtf = this->cur_token().lexeme;
			}
			tsk.as_success(Node::unique_ptr(tk, idtf));
		}
		return;
	};

	this->deduce_statement(prc + 1, tsk, idtf);
	if (tsk.failed) {
		return;
	}

	std::unique_ptr<Operable> superior = std::move(tsk.value);
	this->advance();

	Token op = this->cur_token();
	while (op.valid && op.lexeme.precedence == prc && op.lexeme.binary()) {
		this->advance();
		if (this->cur_token().lexeme.type == LexemeDevice::eof) {
			return tsk.as_failure(RuntimeError::unique_ptr(op.position, "Unexpected EOF while parsing"));
		}

		this->deduce_statement(prc + 1, tsk, op.lexeme);
		if (tsk.failed) {
			return;
		}

		superior = BiNode::unique_ptr(superior, op, tsk.value);
		this->advance();
		op = this->cur_token();
	};

	// having not reached the end of tokens in the end of
	// the initiation call is indicative of an invalid syntax`
	if (prc == -1 && this->index != this->tokens.size() - 1) {
		return tsk.as_failure(RuntimeError::unique_ptr(this->cur_token().position));
	}

	// compensate an imminent overstep in parsing
	this->retreat();
	return tsk.as_success(superior);
};