#include "virtual.cch.h"


    /* virtual */ void impl::doSomething() { /* do nothing */ }
    /* virtual */ int impl::calcSomething(int x, int y) /* override */ {
        return x * y;
    }
