#pragma once


// This test case makes no attempt to be valid C++ code after processing.
// Rather it tests that all of the 38 operator overloads, as well as
// type conversion operator overloads, are properly handled.

class OverrideCity {
public:

    void operator +(size_t i);
    void operator +=(size_t i);
    void operator ++(size_t i);

    void operator -(size_t i);
    void operator -=(size_t i);
    void operator --(size_t i);

    void operator *(size_t i);
    void operator *=(size_t i);

    void operator /(size_t i);
    void operator /=(size_t i);

    void operator %(size_t i);
    void operator %=(size_t i);

    void operator ^(size_t i);
    void operator ^=(size_t i);

    void operator &(size_t i);
    void operator &=(size_t i);
    void operator &&(size_t i);

    void operator |(size_t i);
    void operator |=(size_t i);
    void operator ||(size_t i);

    void operator !(size_t i);
    void operator !=(size_t i);

    void operator ~(size_t i);

    void operator =(size_t i);
    void operator ==(size_t i);

    void operator <(size_t i);
    void operator <=(size_t i);
    void operator <<(size_t i);
    void operator <<=(size_t i);

    void operator >(size_t i);
    void operator >=(size_t i);
    void operator >>(size_t i);
    void operator >>=(size_t i);

    void operator ,(size_t i);

    void operator ->*(size_t i);
    void operator ->(size_t i);


    void operator (  )(size_t i);
    void operator [ /*foobar*/  ](size_t i);

    OverrideCity& operator/ /* awkwardly placed comment */ ();

    // Test the type conversion operator.
    operator bool() const;
    explicit operator int*() const;

    // Test C++11 user-defined literals.
    double operator "" _foo(const char* str);
};

