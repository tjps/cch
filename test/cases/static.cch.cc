#include "static.cch.h"

/* static */ int x = 0;

/* static */ int foo() {
    int m = 1;
    return m * 2;
}
    /* static */ const int bar::cid = 347;
    /* static */ const map<string, string> bar::m = { { "{", "}" } };
    /* static */ const int bar::fnc = foo();
