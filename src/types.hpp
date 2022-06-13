#pragma once

#include <string>
#include <vector>
#include <memory>

#include "lexer.hpp"


class MlAffix;

class MlStr;

class MlObject
{
	std::string m_name;

protected:
	MlObject(std::string nm) : m_name(nm) {};

public:
	// make type specific representations
	virtual std::string repr()
	{
		return this->m_name;
	};
	
	virtual std::unique_ptr<MlObject> operate(std::shared_ptr<MlObject> other, const Token& op) = 0;

	virtual std::unique_ptr<MlObject> operate(const Token& op) = 0;
};

class MlInt : public MlObject
{
public:
	int val;

	static inline std::unique_ptr<MlInt> unique_ptr(std::string characters) {
		return std::make_unique<MlInt>(characters);
	}

	static inline std::shared_ptr<MlInt> shared_ptr(std::string characters) {
		return std::make_shared<MlInt>(characters);
	}

	MlInt(int vl) : MlObject("int"), val(vl) {};

	MlInt(std::string vl) : MlObject("int"), val(std::stoi(vl)) {};

	std::unique_ptr<MlObject> operate(std::shared_ptr<MlObject> other, const Token& op) override;

	std::unique_ptr<MlObject> operate(const Token& op) override;

	std::string repr() override;
};
