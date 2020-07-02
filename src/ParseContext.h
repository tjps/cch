#ifndef __PARSECONTEXT_H__
#define __PARSECONTEXT_H__

#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Holds various pieces of context about the parse.
//
// Most importantly holds the accumulated .cc and .h outputs,
// but also tracks current class scope, whether or not
// the current scope is templated<>, and emits
// #line directives if needed.
//
class ParseContext {
    struct ScopeEntry {
        string name;
        bool templated;
    };
    vector<ScopeEntry> scope;

    const string cchFile;
    const bool emitLineNumbers;

    ostream* ccfile;
    ostream* hfile;

public:

    ParseContext(const string& cchFilename,
            ostream* ccOutputStream,
            ostream* hOutputStream,
            bool _emitLineNumbers)
        : cchFile(cchFilename),
          emitLineNumbers(_emitLineNumbers),
          ccfile(ccOutputStream),
          hfile(hOutputStream) {

        cc() << "#include \"" << filename(cchFile) << ".h\"" << endl;
        h() << "#pragma once" << endl << endl;
    }

    ~ParseContext() {
        h() << endl;
        cc() << endl;
    }

    ostream& cc() {
        return *ccfile;
    }

    ostream& h() {
        return *hfile;
    }

    // Get the current scope in connected form, with trailing '::'.
    // e.g. if inside class B inside namespace A, return "A::B::"
    //   or if at default scope, return ""
    string getScope() const {
        string ret;
        for (int i = 0; i < scope.size(); i++) {
            ret += scope[i].name + "::";
        }
        return ret;
    }

    void pushScope(const string& className, bool templated) {
        ScopeEntry entry;
        entry.name = className;
        entry.templated = templated;
        scope.push_back(entry);
    }

    void popScope() {
        assert(!scope.empty());
        scope.pop_back();
    }

    // Returns true if anything in the scope stack
    // is marked as being templated - all inner classes/scopes
    // are treated as implicitly templated since we don't do
    // actual symbol type processing to see if the template
    // variables are used.
    bool templated() const {
        for (int i = 0; i < scope.size(); i++) {
            if (scope[i].templated) {
                return true;
            }
        }
        return false;
    }

    // If emitting #line directives is requested, then
    // emit a #line directive for the given line number
    // to both the .cc and the .h buffers.
    void emitLineDirective(int lineno) {
        if (emitLineNumbers) {
            stringstream directive;
            directive << "\n#line " << lineno <<
                " \"" << cchFile << "\"\n";
            cc() << directive.str();
            h() << directive.str();
        }
    }

private:
    static string filename(const string& path) {
        size_t end = path.size();
        for (; 0 < end && path[end-1] != '/'; end--);
        return path.substr(end);
    }
};

#endif //__PARSECONTEXT_H__
