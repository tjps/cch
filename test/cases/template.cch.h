#ifndef __test_cases_template_cch__
#define __test_cases_template_cch__


template<class T>
class templatedClass {
    int x(T y) {
    }
};

class c {
    template<typename X>
    X min(X a, X b) {
        return (a < b) ? a : b;
    }
};

template<typename M = map<string, string> >
class forward {};

#endif
