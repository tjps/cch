#ifndef __PARSECONTEXT_H__
#define __PARSECONTEXT_H__

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <assert.h>

// Holds various pieces of context about the parse.
// Most importantly holds the accumulated .cc and .h outputs,
// but also tracks current class scope, whether or not
// the current scope is templated<>, and emits #line directives
// if needed.
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
        string guard = "__" + cchFile + "__";
        // Replace all non-alphanumeric characters in the include guard with '_'
        for (int i = 0; i < guard.size(); i++) {
            if (!isalpha(guard[i]) && !isdigit(guard[i])) {
                guard[i] = '_';
            }
        }
        cc() << "#include \"" << cchFile << ".h\"" << endl;
        h() << "#ifndef " << guard << endl <<
            "#define " << guard << endl;
    }

    ~ParseContext() {
        h() << endl << "#endif" << endl;
        cc() << endl;
    }

    ostream& cc() {
        return *ccfile;
    }

    ostream& h() {
        return *hfile;
    }

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

    bool templated() const {
        for (int i = 0; i < scope.size(); i++) {
            if (scope[i].templated) {
                return true;
            }
        }
        return false;
    }

    void emitLineDirective(int lineno) {
        if (emitLineNumbers) {
            stringstream directive;
            directive << "\n#line " << lineno << " \"" << cchFile << "\"\n";
            cc() << directive.str();
            h() << directive.str();
        }
    }
};

#endif //__PARSECONTEXT_H__
