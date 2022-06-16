#pragma once

#include <map>
#include <string>
#include <memory>

#include "types.hpp"

class Stack
{
	std::map<std::string, std::shared_ptr<MlObject>> symb_table;

public:
	void define(std::string symbol, std::shared_ptr<MlObject>& value);

	std::shared_ptr<MlObject> get_definition(std::string symbol);
};

