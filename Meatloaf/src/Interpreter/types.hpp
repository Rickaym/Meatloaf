#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../lexer.hpp"
#include "../result.hpp"

struct MlObject;

typedef Result<std::shared_ptr<MlObject>> operate_result;

enum MlType {
	unknown,
	mlint,
	mlstr
};

std::string get_mltype_repr(MlType type);

MlType get_repr_mltype(std::string repr);

struct MlObject
{
	MlType type;

protected:
	MlObject(MlType nm) : type(nm) {};

public:
	// make type specific representations
	virtual std::string repr() = 0;

	inline MlType get_type() {
		return this->type;
	}

	virtual bool modify(std::string modifier) = 0;

	virtual operate_result operate(std::shared_ptr<MlObject> other, const Token& op) = 0;

	virtual operate_result operate(const Token& op) = 0;
};

struct MlInt : public MlObject
{
	int val;

	static inline std::unique_ptr<MlInt> unique_ptr(std::string characters) {
		return std::make_unique<MlInt>(std::stoi(characters));
	}

	static inline std::unique_ptr<MlInt> unique_ptr(int v) {
		return std::make_unique<MlInt>(v);
	}

	static inline std::shared_ptr<MlInt> shared_ptr(std::string characters) {
		return std::make_shared<MlInt>(characters);
	}

	static inline std::shared_ptr<MlInt> shared_ptr(int v) {
		return std::make_shared<MlInt>(v);
	}

	MlInt(int vl) : MlObject(MlType::mlint), val(vl) {};

	MlInt(std::string vl) : MlObject(MlType::mlint), val(std::stoi(vl)) {};

	bool modify(std::string modifier) override;

	operate_result operate(std::shared_ptr<MlObject> other, const Token& op) override;

	operate_result operate(const Token& op) override;

	std::string repr() override;
};

struct MlStr : public MlObject
{
	std::string val;

	static inline std::unique_ptr<MlStr> unique_ptr(std::string characters) {
		return std::make_unique<MlStr>(characters);
	}

	static inline std::shared_ptr<MlStr> shared_ptr(std::string characters) {
		return std::make_shared<MlStr>(characters);
	}

	MlStr(int vl) : MlObject(MlType::mlstr), val(std::to_string(vl)) {};

	MlStr(std::string vl) : MlObject(MlType::mlstr), val(vl) {};

	bool modify(std::string modifier) override;

	operate_result operate(std::shared_ptr<MlObject> other, const Token& op) override;

	operate_result operate(const Token& op) override;

	std::string repr() override;
};
