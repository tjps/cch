#include <sstream>
#include "Token.h"

static string replaceAll(string str, const string& from, const string& to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

string Token::toString() const {
    stringstream ss;
    ss << "Token[" << typeToString(type) << ", \"" <<
        replaceAll(value.toString(), "\n", "\\n") << "\"]";
    return ss.str();
}

/* static */ string Token::typeToString(TokenEnum type) {
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
    // Keep older versions of g++ happy that fail to deduce
    // that assert(false) implies the function doesn't return.
    return "";
}
