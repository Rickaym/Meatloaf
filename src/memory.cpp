#include <map>
#include <string>
#include <memory>

#include "memory.hpp"
#include "types.hpp"


void Stack::define(std::string symbol, std::shared_ptr<MlObject>& value) {
	symb_table.insert_or_assign(symbol, std::dynamic_pointer_cast<MlObject>(value));
}

std::shared_ptr<MlObject> Stack::get_definition(std::string symbol) {
	auto search = symb_table.find(symbol);
	if (search == symb_table.end()) {
		return nullptr;
	}
	else {
		return search->second;
	}
}