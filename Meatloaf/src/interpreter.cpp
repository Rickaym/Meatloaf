#include <memory>

#include "interpreter.hpp"


namespace MlInterpreter {
	eval_result eval(std::shared_ptr<Node>& node, Stack& stack)
	{
		std::shared_ptr<MlObject> obj_ptr;
		bool void_eval = false;
		switch (node->token.lexeme.type) {
		case(LexemeDevice::num):
			obj_ptr = MlInt::shared_ptr(node->token.lexeme.characters);
			break;
		case(LexemeDevice::characters):
			obj_ptr = MlStr::shared_ptr(node->token.lexeme.characters);
			break;
		case(LexemeDevice::symbol):
			obj_ptr = stack.get_definition(node->token.lexeme.characters);
			break;
		case(LexemeDevice::eof):
			void_eval = true;
		}

		if (void_eval) {
			return eval_result::success(nullptr);
		}
		else if (obj_ptr == nullptr) {
			return eval_result::failure(
				BaseException::unique_ptr(node->token.position, "BaseException: Lexeme device '" + get_device_repr(node->token.lexeme.type) + "' is not evaluable."));
		}

		if (node->identifier.positional == LexemePositional::suffix &&
			!obj_ptr->modify(node->identifier.characters)) {
			return eval_result::failure(
				// Here try to make it so that the ending position is the end of the modifier or make a compound
				BaseException::unique_ptr(node->token.position, "UnsupportedModifier: Modifier '" + node->identifier.characters + "' is not supported by type '" + get_device_repr(node->token.lexeme.type) + "'."));
		}
		else {
			return eval_result::success(std::move(obj_ptr));
		}
	}

	eval_result eval(std::shared_ptr<BiNode>& node, Stack& stack)
	{
		eval_result inf = eval(node->inferior, stack);
		if (inf.failed) {
			return inf;
		}

		if (node->superior->get_superior_token().lexeme.type == LexemeDevice::symbol && node->op_token.lexeme.characters == "=") {
			// Predefinition
			if (node->superior->get_type() == NodeType::unnode) {
				eval_result predef = eval(node->superior, stack);
				if (predef.failed) {
					return predef;
				}
			}

			if (stack.define(node->superior->get_superior_token().lexeme.characters, inf.value)) {
				return eval_result::success(nullptr);
			}
			else {
				return eval_result::failure(RuntimeError::unique_ptr(
					node->superior->get_superior_token().position,
					"Cannot define variable with type '" + get_mltype_repr(stack.get_type(node->superior->get_superior_token().lexeme.characters)) + "' as '" + get_mltype_repr(inf.value->get_type()) + "'."));
			}
		}
		else {
			eval_result sup = eval(node->superior, stack);
			if (sup.failed) {
				return sup;
			}
			return sup.value->operate(inf.value, node->op_token);
		}
	}

	eval_result eval(std::shared_ptr<UnNode>& node, Stack& stack)
	{
		eval_result oprd = eval(node->operand, stack);
		if (oprd.failed)
		{
			return oprd;
		}
		if (node->operand->get_superior_token().lexeme.type == LexemeDevice::symbol) {
			stack.predefine(node->operand->get_superior_token().lexeme.characters, get_repr_mltype(node->op_token.lexeme.characters));
			return eval_result::success(nullptr);
		}
		else {
			return oprd.value->operate(node->op_token);
		}
	}
}
