#include <string>

struct BaseException 
{
	BaseException(std::string brf) : brief(brf) {};
private:
	std::string brief;
};


struct SyntaxError : public BaseException
{
	SyntaxError() : BaseException("Unexpected character or token found") {};
};
