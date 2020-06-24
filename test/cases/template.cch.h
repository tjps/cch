#pragma once



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

