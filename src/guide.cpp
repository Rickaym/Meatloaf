#include <string>

#include "lexer.hpp"
#include "guide.hpp"

std::string Source::text;
std::string Source::file;

void TypeGuide::advance()
{
    this->pos++;
    this->column++;

    if (this->is_new_line == true)
    {
        this->last_line_max = this->column;
        this->line++;
        this->is_new_line = false;
    };
    this->update_char();
    if (this->chr == '\n')
    {
        this->is_new_line = true;
    };
};

Position TypeGuide::capture()
{
    return Position(this->pos, this->pos, this->column, this->line);
};

void TypeGuide::retreat()
{
    this->pos--;
    this->column--;

    this->update_char();
    if (this->chr == '\n')
    {
        this->column = (int)this->last_line_max;
        this->line--;
    };
}

void TypeGuide::update_char()
{
    if ((unsigned)this->pos < Source::text.length()) {
        this->chr = Source::text[this->pos];
    }
    else {
        this->eof = true;
    }
};

bool TypeGuide::is_delimiter()
{
    const char* uncover = std::find(std::begin(g_lexicon_delimiters),
        std::end(g_lexicon_delimiters), this->chr);
    return !(uncover == std::end(g_lexicon_delimiters));
}

std::string Position::to_string() const
{
    return std::to_string(this->line) + ":" + std::to_string(this->column) + "~" + std::to_string(this->end);
};