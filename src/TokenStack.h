#ifndef __TOKENSTACK_H__
#define __TOKENSTACK_H__

#include <vector>
#include "Token.h"

// Wrapper around a vector of Tokens
// with a few convenience methods.
//
class TokenStack : public vector<Token> {
public:
    ~TokenStack() {
        // If the stack is not empty on destruction
        // it means there are lost tokens that
        // have not been flushed to a stream.
        assert(empty());
    }

    // Returns true if the specified token type
    // is present in the stack.
    bool containsType(TokenEnum type) const {
        for (int i = 0; i < size(); i++) {
            if ((*this)[i].type == type) {
                return true;
            }
        }
        return false;
    }

    // Write all token values to the specified
    // stream and clear the token stack.
    void flushToStream(ostream& stream) {
        for (int i = 0; i < size(); i++) {
            stream << (*this)[i].value;
        }
        clear();
    }

    string toString() const {
        string ret = "TokenStack [";
        for (int i = 0; i < size(); i++) {
            if (i > 0) { ret += " >> "; }
            ret += (*this)[i].toString();
        }
        return ret + "]";
    }
};

#endif //__TOKENSTACK_H__
