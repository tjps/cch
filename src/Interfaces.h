#ifndef __INTERFACES_H__
#define __INTERFACES_H__

struct Token;
struct Location;
class StringView;

class Parser {
public:
    virtual ~Parser() {}
    virtual void acceptToken(const Token& token) = 0;
};

class Tokenizer {
public:
    virtual ~Tokenizer() {}
    virtual void tokenize(const StringView& code, Parser* emitter, Location start) = 0;
};

#endif //__INTERFACES_H__
