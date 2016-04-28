#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <string>
#include <sstream>
#include "StringView.h"
#include "util.h"

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

    string toString() const {
        stringstream ss;
        ss << "Token[" << typeToString(type) << ", \"" <<
            replaceAll(value.toString(), "\n", "\\n") << "\"]";
        return ss.str();
    }

    static string typeToString(TokenEnum type) {
        switch (type) {
        case INVALIDTOKEN: return "INVALIDTOKEN";
        case TOKEN: return "TOKEN";
        case CLASS: return "CLASS";
        case ASSIGN: return "ASSIGN";
        case STRING_LITERAL: return "STRING_LITERAL";
        case COMMENT: return "COMMENT";
        case PREPROC: return "PREPROC";
        case SEMICOLON: return "SEMICOLON";
        case COLON: return "COLON";
        case BRACE_GROUP: return "BRACE_GROUP";
        case PARENS_GROUP: return "PARENS_GROUP";
        case WHITESPACE: return "WHITESPACE";
        case OPENBRACE: return "OPENBRACE";
        case CLOSEBRACE: return "CLOSEBRACE";
        case TEMPLATE: return "TEMPLATE";
        case USING: return "USING";
        case NAMESPACE: return "NAMESPACE";
        };
        assert(false && "Invalid TokenEnum value");
    }
};

#endif //__TOKEN_H__
