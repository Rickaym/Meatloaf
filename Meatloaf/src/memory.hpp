#pragma once

#include <map>
#include <string>
#include <memory>

#include "types.hpp"

class Stack
{
	std::map<std::string, std::shared_ptr<MlObject>> symb_table;
	std::map<std::string, MlType> type_table;

public:
	bool define(std::string symbol, std::shared_ptr<MlObject>& value);
	
	bool predefine(std::string symbol, MlType type);

	MlType get_type(std::string symbol);
	
	std::shared_ptr<MlObject> get_definition(std::string symbol);
};

