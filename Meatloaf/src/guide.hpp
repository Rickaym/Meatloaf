#pragma once

#if __SIZEOF_POINTER__ == 8
#define ARCH "x86_64"
#else
#define ARCH "x86"
#endif

#if defined(_WIN32)
#define OS "windows"
#elif defined(__linux__)
#define OS "linux"
#elif defined(__APPLE__)
#define OS "macos"
#elif defined(__ANDROID__)
#define OS "android"
#elif defined(__FreeBSD__)
#define OS "freebsd"
#else
#define OS "unknown"
#endif

/**
 ---------- Guide ------------
 Includes complimentary structs for the parser and the lexer that is also
 required by other components. Escapes dependency conflicts.
*/


// fwds

struct Position;

/**
 Source object shared by the different parts of the compiler.
*/
struct Source
{
    static std::string text;
    static std::string file;
};

/**
  A type guide used to feed characters into the lexer
  during tokenization, this isn't part of the lexer class as opposed to
  the parser having it's own internal guide.
*/
struct TypeGuide
{
    int pos = -1;
    char chr = '.';
    int len = 0;
    int column = 0;
    int line = 1;
    int last_line_max = 0;
    bool is_new_line = false;
    bool eof = false;

    TypeGuide() = default;

    void advance();

    void retreat();

    bool is_delimiter();

    Position capture();

private:
    void update_char();
};

/**
 A description of the position a morpheme has in the source code.
*/
struct Position
{
    int start = 0;
    int end = 0;
    int column = 1;
    int line = 1;

    Position() = default;

    Position(int start, int end, int column, int line) : start(start), end(end),
        column(column), line(line) {};

    Position(TypeGuide guide) : Position(guide.pos, guide.pos + 1, guide.column, guide.line) {};

    std::string to_string() const;
};
