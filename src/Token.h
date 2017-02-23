#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "StringView.h"

enum TokenEnum {
    INVALIDTOKEN, TOKEN, CLASS, ASSIGN, STRING_LITERAL, COMMENT, PREPROC,
    SEMICOLON, COLON, BRACE_GROUP, PARENS_GROUP, WHITESPACE,
    OPENBRACE, CLOSEBRACE, TEMPLATE, USING, NAMESPACE
};

struct Location {
    size_t pos;    // absolute index into string. 0-indexed.
    size_t line;   // line number in file.        1-indexed.
    size_t column; // character column in file.   0-indexed.
    Location() : pos(0), line(1), column(0) {}
};

struct Token {
    StringView value;
    TokenEnum type;
    Location start;
    Location end;

    Token(StringView _value, TokenEnum _type,
          Location _start, Location _end)
        : value(_value), type(_type), start(_start), end(_end) {}

    string toString() const;

    // Turn a TokenEnum value into a corresponding string.
    static string typeToString(TokenEnum type);
};

#endif //__TOKEN_H__
