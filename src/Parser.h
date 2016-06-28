#ifndef __PARSER_H__
#define __PARSER_H__

#include "Interfaces.h"
#include "TokenStack.h"
#include "Keywords.h"
#include "ParseContext.h"

// Simple parser wrapper that transforms certain keywords
// into their corresponding token types.
//
class WrapperParser : public Parser {
    Parser& mWrapped;
public:
    WrapperParser(Parser& wrapped)
        : mWrapped(wrapped) {}

    void acceptToken(const Token& token) {
        TokenEnum type = token.type;
        if (token.type == TOKEN) {
            if (token.value == "class"
                || token.value == "struct"
                || token.value == "union") {
                type = CLASS;
            } else if (token.value == "template") {
                type = TEMPLATE;
            } else if (token.value == "using") {
                type = USING;
            } else if (token.value == "namespace") {
                type = NAMESPACE;
            }
        }
        if (type == token.type) {
            mWrapped.acceptToken(token);
        } else {
            Token tok = token;
            tok.type = type;
            mWrapped.acceptToken(tok);
        }
    }
};

// This parser evaluates the token stack each time a token is added,
// allowing for the stack to be reduced as soon as a pattern is matched.
//
class BaseParser : public Parser {
    TokenStack mTokens;
    Tokenizer* mTokenizer;  // The tokenizer used to recurse into class bodies.
    ParseContext* mCtx;

public:
    BaseParser(ParseContext* ctx, Tokenizer* tokenizer)
        : mTokenizer(tokenizer), mCtx(ctx) {}

    ~BaseParser() {
        evalTokenStack();
        finalize();
    }

    void acceptToken(const Token& token) {
        mTokens.push_back(token);
        evalTokenStack();
    }

private:
    // Equivalent to yacc in a lex/yacc configuration.
    // Evaluate the current token stack to see what can be reduced/emitted.
    void evalTokenStack() {

        // List of remaining C++(11) features to be handled:
        // TODO: handle default params.
        // TODO: Complete the set of operator** overloads.
        // TODO: Add handling for throw() decorators.
        // TODO: Add handling for alignas().
        // TODO: Add handling for try constructors.

        // Possible regex form equivalents:
        // (COMMENT|WHITESPACE)* PREPROC
        // (COMMENT|WHITESPACE)* TOKEN (COMMENT|WHITESPACE)* COLON

        if (mTokens.empty()) {
            // Do nothing, but guard all future conditionals from needing to check empty().
        } else if (mTokens.back().type == PREPROC) {
            mTokens.flushToStream(mCtx->h());
        } else if (mTokens.back().type == COLON) {
            bool isLabel = true;
            // Should be only comments or whitespace before the 'keyword'':' pair.
            for (int i = 0; i < mTokens.size()-2; i++) {
                isLabel &= (mTokens[i].type == COMMENT || mTokens[i].type == WHITESPACE);
            }
            if (isLabel) {
                // Flush the label out to the header.
                mTokens.flushToStream(mCtx->h());
            }
        } else if (mTokens.back().type == SEMICOLON) {   // Handle general statements.
            bool splitAssignmentToCCFile = mTokens.containsType(ASSIGN)
                && !mTokens.containsType(USING);
            if (splitAssignmentToCCFile) {
                for (int i = 0; i < mTokens.size() && mTokens[i].type != ASSIGN; i++) {
                    if (mTokens[i].type == PARENS_GROUP) {
                        splitAssignmentToCCFile = (i + 1 < mTokens.size() && mTokens[i+1].type == PARENS_GROUP);
                        break;
                    }
                }
            }
            if (!splitAssignmentToCCFile) {
                for (int i = 0; i < mTokens.size() && mTokens[i].type != ASSIGN; i++) {
                    if (mTokens[i].value == "static") {
                        splitAssignmentToCCFile = true;
                        break;
                    }
                }
            }
            if (splitAssignmentToCCFile) {
                mCtx->emitLineDirective(mTokens[0].start.line);
                int i = 0;
                for (; i < mTokens.size() && mTokens[i].type != ASSIGN; i++);
                for (; i-1 >= 0 && mTokens[i-1].type == WHITESPACE; i--);
                int splitPoint = i;
                for (i--; i >= 0 && mTokens[i].type != TOKEN; i--);
                int identifier = i;
                for (i = 0; i < splitPoint; i++) {
                    mCtx->h() << mTokens[i].value;
                    if (i == identifier) {
                        mCtx->cc() << mCtx->getScope() << mTokens[i].value;
                    } else if (mTokens[i].value != "static") {
                        mCtx->cc() << mTokens[i].value;
                    }
                }
                mCtx->h() << ";";
                for (; i < mTokens.size(); i++) {
                    mCtx->cc() << mTokens[i].value;
                }
                mCtx->emitLineDirective(mTokens.back().end.line);
                mTokens.clear();
            } else {
                mTokens.flushToStream(mCtx->h());
            }
        } else if (mTokens.back().type == BRACE_GROUP
                   && (mTokens.containsType(CLASS) || mTokens.containsType(NAMESPACE))) {
            bool templated = false;
            string scopeName;
            for (int i = 0; i+1 < mTokens.size(); i++) {
                if (mTokens[i].type == CLASS || mTokens[i].type == NAMESPACE) {
                    for (i++; i < mTokens.size() && mTokens[i].type == WHITESPACE; i++);
                    if (i < mTokens.size() && mTokens[i].type == TOKEN) {
                        scopeName = mTokens[i].value.toString();
                    }
                    break;
                } else if (mTokens[i].type == TEMPLATE) {
                    templated = true;
                }
            }
            mCtx->pushScope(scopeName, templated);
            {
                StringView body = mTokens.back().value;
                Location start = mTokens.back().start;
                assert(body[0] == '{');
                assert(body[body.size()-1] == '}');
                // Trim off outer braces to prevent infinite brace group capture.
                body = body.slice(1, body.size()-1);
                // Remove the brace group from the token stack.
                mTokens.pop_back();
                // Flush the opening of the class/namespace out to header.
                mTokens.flushToStream(mCtx->h());
                mCtx->h() << "{";

                WrapperParser wrapper(*this);
                mTokenizer->tokenize(body, &wrapper, start);

                mTokens.flushToStream(mCtx->h());
                mCtx->h() << "}";
            }
            mCtx->popScope();
        } else if (mTokens.back().type == BRACE_GROUP) { // Handle functions with bodies.
            int i = 0;
            bool keepInHeader = false;
            int identifier = -1;
            for (; i < mTokens.size() && (mTokens[i].type == TOKEN ||
                                          mTokens[i].type == WHITESPACE ||
                                          mTokens[i].type == COMMENT ||
                                          mTokens[i].type == TEMPLATE); i++) {
                if (Keywords.isHeaderOnly(mTokens[i].value)
                    || mTokens[i].type == TEMPLATE) {
                    keepInHeader = true;
                }
                if (mTokens[i].type == TOKEN) {
                    identifier = i;
                }
            }

            if (mTokens[i].type == PARENS_GROUP) {
                for (i++; i < mTokens.size() &&
                         (mTokens[i].type == TOKEN || mTokens[i].type == COMMENT || mTokens[i].type == WHITESPACE); i++);
                int initializerList = -1;
                if (i < mTokens.size() && mTokens[i].type == COLON) {
                    initializerList = i;
                    // Walk over initializer list.
                    for (i++; i < mTokens.size() &&
                             (mTokens[i].type == TOKEN || mTokens[i].type == COMMENT || mTokens[i].type == WHITESPACE || mTokens[i].type == PARENS_GROUP); i++);
                }
                if (i + 1 == mTokens.size()) {
                    // We have a function with body!
                    if (mCtx->templated() || keepInHeader) {
                        mTokens.flushToStream(mCtx->h());
                    } else {
                        mCtx->emitLineDirective(mTokens[0].start.line);
                        int headerStop = (initializerList != -1)
                            ? initializerList
                            : mTokens.size() - 1;
                        for (; headerStop - 1 >= 0 && mTokens[headerStop-1].type == WHITESPACE; headerStop--);
                        for (i = 0; i < headerStop; i++) {
                            mCtx->h() << mTokens[i].value;
                            if (i == identifier) {
                                mCtx->cc() << mCtx->getScope() << mTokens[i].value;
                            } else if (!Keywords.isStrippedFromDefinition(mTokens[i].value)) {
                                mCtx->cc() << mTokens[i].value;
                            }
                        }
                        mCtx->h() << ";";
                        for (; i < mTokens.size(); i++) {
                            mCtx->cc() << mTokens[i].value;
                        }
                        mCtx->emitLineDirective(mTokens.back().end.line);
                        mTokens.clear();
                    }
                }
            }
        }
    }

    void finalize() {
        // Make sure any remaining tokens are either comments or whitespace,
        // since they are flushed only to the header.
        for (int i = 0; i < mTokens.size(); i++) {
            if (mTokens[i].type != COMMENT && mTokens[i].type != WHITESPACE) {
                cerr << "ERROR: exiting with unconsumed tokens." <<
                    " Tokens: " << mTokens.toString() << endl <<
                    "Please open an issue at https://github.com/tjps/cch " <<
                    "including source .cch, if possible." << endl;
                abort();
            }
        }
        mTokens.flushToStream(mCtx->h());
    }
};

#endif //_PARSER_H__
