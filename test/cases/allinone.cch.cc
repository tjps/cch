#include "allinone.cch.h"


/* WORD WORD ASSIGNMENT WORD */
int b = 1;

/* WORD WORD ASSIGNMENT bracegroup */
std::vector<std::string> strs = { "abc", "def" };

/* WORD parengroup parengroup ASSIGNEMENT WORD WORD */
void (*fn2)(const char*, int) = &foo;

/* WORD WORD parengroup bracegroup */
bool fn(const string& str) {
    return str.empty();
}

std::function<void(int, char*)> lambda = [](int a, char* b){ a++; };
    int a::foo(int x, int y) {
        return x * y;
    }
