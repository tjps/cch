#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <vector>
#include "Interfaces.h"
#include "Token.h"
#include "StringView.h"

// This tokenizer iterates over the input text, splitting it into
// appropriate instances of Token and sending them to the specified emitter.
//
class BaseTokenizer : public Tokenizer {
    class TokenTracker;

public:
    void tokenize(const StringView& code,
                  Parser* emitter,
                  Location start = Location()) {
        TokenTracker token(code, emitter, start);
        tokenize(code, token);
        assert(token.getBytesConsumed() == code.size());
    }

private:

    // Tokenize the input string, accumulating and emitting tokens
    // through the TokenTracker.
    void tokenize(const StringView& code, TokenTracker& token) {

        static struct {
            TokenizerState getStateForChar(char c) const {
                switch (c) {
                case '{': case '}': return BRACE_CAPTURE;
                case '(': case ')': return PARENS_CAPTURE;
                case '[': case ']': return BRACKET_CAPTURE;
                case '<': case '>': return ANGLE_CAPTURE;
                default: assert(false && "No state for specified char");
                    // Keep older versions of g++ happy that fail to deduce
                    // that assert(false) implies the function doesn't return.
                    return INVALIDSTATE;
                }
            }

            TokenEnum getTokenForChar(char c) const {
                switch (c) {
                case '{': case '}': return BRACE_GROUP;
                case '(': case ')': return PARENS_GROUP;
                case '[': case ']': return TOKEN;
                case '<': case '>': return TOKEN;
                default: assert(false && "No token type for specified char");
                    // Keep older versions of g++ happy that fail to deduce
                    // that assert(false) implies the function doesn't return.
                    return INVALIDTOKEN;
                }
            }
        } CharMap;

        StateStack states;

        // This is the analog to the lexer in a traditional lex/yacc configuration.
        // This loop consumes the input and pushes tokens onto the token stack.
        for (size_t i = 0; i < code.size(); i++) {
            token.setEnd(i);
            if (states.currentState() == NORMAL
                && token.get() == "operator") {
                // This still feels hackish, but I have
                // yet to find a more elegant solution.
                states.pushState(OPERATOR);
            }
            switch (states.currentState()) {
            case NORMAL:
                switch (code[i]) {
                case '\'':
                case '"':
                    token.flush();
                    states.pushState(LITERAL_CAPTURE);
                    states.setCurrentStateMatchChar(code[i]);
                    break;
                case '/':
                    if (i+1 < code.size()) {
                        if (code[i+1] == '*') {
                            token.flush();
                            states.pushState(C_COMMENT);
                            i++;
                        } else if (code[i+1] == '/') {
                            token.flush();
                            states.pushState(CPP_COMMENT);
                            i++;
                        }
                    }
                    break;
                case '<':
                    if (code[i] == '<' && i+1 < code.size() && code[i+1] == '<') {
                        i++; // Skip over left-shift operator '<<' as just a normal token.
                        break;
                    }
                case '{':
                case '(':
                case '[':
                    token.flush();
                    states.pushState(CharMap.getStateForChar(code[i]));
                    break;
                case '=':
                    token.emitToken("=", ASSIGN);
                    break;
                case ':':
                    if (i + 1 < code.size() && code[i+1] == ':') {
                        i++; // Skip '::' pairs.
                    } else {
                        token.emitToken(":", COLON);
                    }
                    break;
                case ';':
                    token.emitToken(";", SEMICOLON);
                    break;
                case '#':
                    // Only switch to preprocessor mode if this '#'
                    // is the first character of the token.
                    if (token.empty()) {
                        states.pushState(PREPROCESSOR);
                    }
                    break;
                case ' ':
                case '\n':
                case '\r':
                case '\t':
                case '\v':
                case '\f':
                    token.flush();
                    // Capture the entire span of whitespace.
                    for (; i+1 < code.size() && isspace(code[i+1]); i++);
                    token.setEnd(i+1);
                    token.flush(WHITESPACE);
                    break;
                }
                break;
            case PREPROCESSOR:
                if (code[i] == '\n' && code[i-1] != '\\') {
                    token.setEnd(i+1);
                    if (states.currentStateEmit()) {
                        token.flush(PREPROC);
                    }
                    states.popState();
                }
                break;
            case LITERAL_CAPTURE:
                if (code[i] == '\\' && i+1 < code.size()) {
                    i++; // Skip escaped character.
                } else if (code[i] == states.getCurrentStateMatchChar()) {
                    token.setEnd(i+1);
                    if (states.currentStateEmit()) {
                        token.flush(STRING_LITERAL);
                    }
                    states.popState();
                }
                break;
            case C_COMMENT:
                if (i+1 < code.size() && code[i] == '*' && code[i+1] == '/') {
                    i++; // The terminal '*/' is two characters long,
                    // so skip forward one to capture the full terminal.
                    token.setEnd(i+1);
                    if (states.currentStateEmit()) {
                        token.flush(COMMENT);
                    }
                    states.popState();
                }
                break;
            case CPP_COMMENT:
                if (code[i] == '\n') {
                    token.setEnd(i+1);
                    if (states.currentStateEmit()) {
                        token.flush(COMMENT);
                    }
                    states.popState();
                }
                break;
            case BRACE_CAPTURE:
            case PARENS_CAPTURE:
            case BRACKET_CAPTURE:
            case ANGLE_CAPTURE:
                switch (code[i]) {
                case '\'':
                case '"':
                    states.pushState(LITERAL_CAPTURE);
                    states.setCurrentStateMatchChar(code[i]);
                    states.currentStateEmit() = false;
                    break;
                case '/':
                    if (i+1 < code.size()) {
                        if (code[i+1] == '*') {
                            states.pushState(C_COMMENT);
                            states.currentStateEmit() = false;
                            i++;
                        } else if (code[i+1] == '/') {
                            states.pushState(CPP_COMMENT);
                            states.currentStateEmit() = false;
                            i++;
                        }
                    }
                    break;
                case '{':
                case '(':
                case '[':
                case '<':
                    if (states.currentState() == CharMap.getStateForChar(code[i])) {
                        states.currentStateDepth()++;
                    }
                    break;
                case '}':
                case ')':
                case ']':
                case '>':
                    if (states.currentState() == CharMap.getStateForChar(code[i])) {
                        // If the depth reaches 0, then this character was the balanced
                        // match to the character that opened this state, so emit and pop.
                        if (--states.currentStateDepth() == 0) {
                            TokenEnum tokenType = CharMap.getTokenForChar(code[i]);
                            token.setEnd(i+1);
                            if (states.currentStateEmit()) {
                                token.flush(tokenType);
                            }
                            states.popState();
                        }
                    }
                    break;
                }
                break;
            case OPERATOR:
                if (states.currentStateDepth() == 347) {
                    token.flush();
                    states.popState();
                    i--; // Unconsume this current unrelated character.
                    break;
                }
                switch (code[i]) {
                case '-': case '+':
                case '*': case '/':
                case '%': case '^':
                case '&': case '|':
                case '!': case '=':
                case '<': case '>':
                    // Handle '->' and '->*' operators.
                    if (code[i] == '-' && i+1 < code.size() && code[i+1] == '>') {
                        i++;
                        if (i+1 < code.size() && code[i+1] == '*') {
                            i++;
                        }
                        // Handle all double operators (e.g. '<<' '&&' ...)
                    } else if ((code[i] == '<' || code[i] == '>' || code[i] == '+'
                                || code[i] == '-' || code[i] == '&' || code[i] == '|')
                               && i+1 < code.size() && code[i+1] == code[i]) {
                        i++;
                    }
                    // Handle the assignment form (e.g. '+=' '>>=' ...)
                    if (i+1 < code.size() && code[i+1] == '=') {
                        i++;
                    }
                case '~':  // These two operators appear only as solo chars.
                case ',':
                    token.setEnd(i+1);
                    token.flush();
                    states.popState();
                    break;
                case '(':
                case '[':
                    // Mark this state to emit next time we get back to it.
                    states.currentStateDepth() = 347;
                    // Switch to a group capture, but do not emit.
                    // This accumulates the group into the current token.
                    states.pushState(CharMap.getStateForChar(code[i]));
                    states.currentStateEmit() = false;
                    break;
                case ' ':
                case '\n':
                case '\r':
                case '\t':
                case '\v':
                case '\f':
                    if (token.get() == "operator") {
                        // If this is the first character in the operator
                        // capture, accumulate all whitespace.
                        for (; i < code.size() && isspace(code[i]); i++);
                    } else {
                        token.flush();
                        states.popState();
                    }
                    i--; // unconsume the last character.
                    break;
                case '"':
                    if (i+1 < code.size() && code[i+1] == '"') {
                        i += 2;
                        // Handle the user-defined literal case.
                        // Capture any whitespace, if present.
                        for (; i < code.size() && isspace(code[i]); i++);
                        // Capture any combo of '_' or alphabetic.
                        for (; i < code.size() && (isalpha(code[i]) || code[i] == '_'); i++);
                        token.setEnd(i);
                        i--;
                    }
                default:
                    if (token.get() != "operator") {
                        // As long as there was at least some whitespace
                        // after 'operator' consider the type conversion
                        // operator case.
                        if (isalpha(code[i]) || code[i] == '_') {
                            for (; i < code.size() && code[i] != '('; i++);
                            token.setEnd(i);
                            i--;
                        }
                    }
                    token.flush();
                    states.popState();
                    break;
                }
                break;
            case INVALIDSTATE: assert(false && "Should never be in INVALIDSTATE");
            }
        }
        if (states.currentState() == LITERAL_CAPTURE
            || states.currentState() == BRACE_CAPTURE
            || states.currentState() == PARENS_CAPTURE
            || states.currentState() == BRACKET_CAPTURE
            || states.currentState() == ANGLE_CAPTURE) {
            // If we are still in any of the capture modes at the end of
            // code block, the input is definitely malformed.
            cout << "Unclosed capture" << endl;
            // TODO: gracefully exit instead of exit(),
            //   and print the location of the offending start character.
            exit(1);
        }
        // Push the remaining token (if any).
        token.setEnd(code.size());
        token.flush();
    }

    // Tracks beginning and end of a subsection of the
    // backing string with the ability to flush that
    // subsection out as a Token to the specified emitter.
    //
    class TokenTracker {
        const StringView mBackingString;
        Parser* mEmitter;

        Location mStart;
        Location mEnd;
        size_t mBytesConsumed;

    public:

        TokenTracker(const StringView& backingString,
                     Parser* emitter, Location start)
            : mBackingString(backingString), mEmitter(emitter),
              mStart(start), mEnd(start), mBytesConsumed(0) {
            // Preserve column/row numbers but set the position
            // (relative to backing string) to 0.
            mStart.pos = mEnd.pos = 0;
        }

        void flush(TokenEnum type = TOKEN) {
            emit(get(), type);
            reset(mEnd.pos);
            assert(empty());
        }

        void emitToken(const StringView& token, TokenEnum type) {
            flush();
            emit(token, type);
            reset(mEnd.pos + token.size());
        }

        void setEnd(size_t index) {
            assert(index <= mBackingString.size());
            assert(mEnd.pos <= index);
            for (size_t i = mEnd.pos; i < index; i++) {
                if (mBackingString[i] == '\n') {
                    mEnd.line++;
                    mEnd.column = 0;
                } else {
                    mEnd.column++;
                }
            }
            mEnd.pos = index;
        }

        size_t size() const {
            return mEnd.pos - mStart.pos;
        }

        bool empty() const {
            return size() == 0;
        }

        StringView get() const {
            return mBackingString.slice(mStart.pos, mEnd.pos);
        }

        size_t getBytesConsumed() const {
            return mBytesConsumed;
        }

    private:
        void reset(size_t index) {
            assert(index <= mBackingString.size());
            assert(mEnd.pos <= index);
            mEnd.pos = index;
            mStart = mEnd;
        }

        void emit(const StringView& token, TokenEnum type) {
            if (token.size() > 0) {
                if (type == TOKEN) {
                    assert(!isspace(token[0]));
                    if (isspace(token[token.size()-1])) {
                        cerr << "Token: '" << token << "'" << endl;
                    }
                    assert(!isspace(token[token.size()-1]));
                }
                mEmitter->acceptToken(Token(token, type, mStart, mEnd));
                mBytesConsumed += token.size();
                if (0) {
                    cout << "emit(" << Token::typeToString(type) << " " <<
                        mStart.line << ":" << mStart.column << " -> " <<
                        mEnd.line << ":" << mEnd.column << "): " <<
                        replaceAll(token.toString(), "\n", "\\n") << endl;
                }
            }
        }
    };

    enum TokenizerState {
        INVALIDSTATE, NORMAL, CPP_COMMENT, C_COMMENT, LITERAL_CAPTURE,
        BRACE_CAPTURE, PARENS_CAPTURE, BRACKET_CAPTURE, ANGLE_CAPTURE,
        PREPROCESSOR, OPERATOR
    };

    // StateStack provides syntactic sugar around a vector
    // of TokenizerStates.  In addition to more informative
    // method names, it also tracks extra state data and
    // enforces a few invariants that then don't need to be
    // checked in the client code.
    //
    class StateStack {

        struct ss {
            TokenizerState state;
            char chr;
            bool chr_set;
            int depth;
            bool emit;
            ss(TokenizerState _state)
                : state(_state),
                  chr(0),
                  chr_set(false),
                  depth(1),
                  emit(true) {}
            ss()
                : state(INVALIDSTATE),
                  chr(0),
                  chr_set(false),
                  depth(1),
                  emit(true) {}
        };

        vector<ss> mStates;

    public:
        StateStack() {
            // Start in the NORMAL state.
            pushState(NORMAL);
        }

        ~StateStack() {
            assert(mStates.size() == 1);
            assert(mStates[0].state == NORMAL);
        }

        void pushState(TokenizerState state) {
            mStates.push_back(ss(state));
        }

        void popState() {
            assert(!mStates.empty());
            mStates.pop_back();
        }

        TokenizerState currentState() const {
            assert(!mStates.empty());
            return mStates.back().state;
        }

        void setCurrentStateMatchChar(char c) {
            assert(!mStates.empty());
            mStates.back().chr_set = true;
            mStates.back().chr = c;
        }

        char getCurrentStateMatchChar() const {
            assert(!mStates.empty());
            assert(mStates.back().chr_set);
            return mStates.back().chr;
        }

        int& currentStateDepth() {
            assert(!mStates.empty());
            return mStates.back().depth;
        }

        bool& currentStateEmit() {
            assert(!mStates.empty());
            return mStates.back().emit;
        }
    };
};

#endif //__TOKENIZER_H__
