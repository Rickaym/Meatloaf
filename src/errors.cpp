#include <string>
#include <sstream>

#include "errors.hpp"
#include "guide.hpp"

std::string BaseException::traceback() const
{
	// we need the starting index of the line to display the full line
	int lnStart = this->position_.start - (this->position_.column - 1);
	// reveals the amount of correct tokens before error actually starts
	int errStart = this->position_.start - lnStart;

	int errAmt = this->position_.end - errStart;
	int othrAmt = errStart - lnStart;

	std::string gutter = std::to_string(this->position_.line) + " | ";
	size_t end = Source::text.find_first_of("\n", lnStart);
	return "Exception in " + Source::file + ":\n" +
		gutter + Source::text.substr(lnStart, end) + '\n' +
		std::string(gutter.size(), ' ') + std::string(othrAmt, '~') +
		std::string(errAmt, '^') + '\n' +
		this->brief_;
};

std::ostream& operator<<(std::ostream& os, const BaseException& n)
{
	os << n.traceback();
	return os;
}