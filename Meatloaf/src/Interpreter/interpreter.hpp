#pragma once

#include <memory>

#include "../parser.hpp"
#include "memory.hpp"

namespace MlInterpreter {
	typedef Result<std::shared_ptr<MlObject>> eval_result;

	eval_result eval(std::shared_ptr<Node>& node, Stack& stack);
	
	eval_result eval(std::shared_ptr<BiNode>& node, Stack& stack);

	eval_result eval(std::shared_ptr<UnNode>& node, Stack& stack);

	inline eval_result eval(std::shared_ptr<Operable>& obj, Stack& stack) {
		switch (obj->get_type()) {
		case NodeType::node:
			return MlInterpreter::eval(std::dynamic_pointer_cast<Node>(obj), stack);
		case NodeType::binode:
			return MlInterpreter::eval(std::dynamic_pointer_cast<BiNode>(obj), stack);
		case NodeType::unnode:
			return MlInterpreter::eval(std::dynamic_pointer_cast<UnNode>(obj), stack);
		}
	}
}