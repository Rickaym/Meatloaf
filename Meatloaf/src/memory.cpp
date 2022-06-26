#include <map>
#include <string>
#include <memory>

#include "memory.hpp"
#include "types.hpp"


bool Stack::define(std::string symbol, std::shared_ptr<MlObject>& value) {
	auto search = type_table.find(symbol);
	
	// Trying to set a defined variable to a different type
	if (search != type_table.end() && value->get_type() != search->second) {
		return false;
	} 
	else {
		type_table.insert(std::make_pair(symbol, value->get_type()));
	}
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

bool Stack::predefine(std::string symbol, MlType type) {
	auto search = type_table.find(symbol);
	if (search != type_table.end()) {
		return false;
	}
	else {
		type_table.insert(std::make_pair(symbol, type));
		return true;
	}
}

MlType Stack::get_type(std::string symbol) {
	auto search = type_table.find(symbol);
	if (search == type_table.end()) {
		return MlType::unknown;
	}
	else {
		return search->second;
	}
}