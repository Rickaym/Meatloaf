#include <string>
#include <sstream>

#include "errors.h"
#include "guide.h"

std::string BaseException::traceback() const
{
	// true line start
	int lnStart = this->position_.start - (this->position_.column - 1);
	int errStart = this->position_.start - lnStart;
	
	std::string snippet = Source::text.substr(lnStart, this->position_.end);
	std::string gutter = std::to_string(this->position_.line) + " | ";
	
	return "Exception in " + Source::uri + ":\n" +
		gutter + snippet + '\n' +
		std::string(gutter.size(), ' ') + std::string(errStart - lnStart, '~') +
		std::string((long)this->position_.end - errStart, '^') + '\n' +
		this->brief_;
};

std::ostream& operator<<(std::ostream& os, const BaseException& n)
{
	os << n.traceback();
	return os;
}