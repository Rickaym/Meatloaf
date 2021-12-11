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
	std::unique_ptr<MlObject> getPrivates(std::string attrname);
	
	virtual std::unique_ptr<MlStr> repr()
	{
		return std::make_unique<MlStr>(this->m_name);
	};

	template<typename T>
	T getMetaValue() = 0;

	/* Binary operate */
	virtual std::unique_ptr<MlObject> operate(const std::unique_ptr<MlObject>& other, const Token& op)
	{
		return nullptr;
	};

	/* Unary operate */
	virtual std::unique_ptr<MlObject> operate(const Token& op)
	{
		return nullptr;
	};
};

class MlStr : public MlObject
{
public:
	std::string val;

	MlStr(std::string str) : MlObject("str"), val(str) {};

	std::unique_ptr<MlObject> operate(const std::unique_ptr<MlObject>& other, const Token& op) override;
};

class MlInt : public MlObject
{
public:
	int val;

	MlInt(int vl) : MlObject("int"), val(vl) {};

	MlInt(std::string vl) : MlObject("int"), val(std::stoi(vl)) {};

	std::unique_ptr<MlObject> operate(const std::unique_ptr<MlObject>& other, const Token& op) override;
};

class MlAffix : public MlObject
{
public:
	MlAffix() : MlObject("affix") {};
};

