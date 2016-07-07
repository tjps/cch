#include "tests/operators.cch.h"


    void OverrideCity::operator +(size_t i) {}
    void OverrideCity::operator +=(size_t i) {}
    void OverrideCity::operator ++(size_t i) {}

    void OverrideCity::operator -(size_t i) {}
    void OverrideCity::operator -=(size_t i) {}
    void OverrideCity::operator --(size_t i) {}

    void OverrideCity::operator *(size_t i) {}
    void OverrideCity::operator *=(size_t i) {}

    void OverrideCity::operator /(size_t i) {}
    void OverrideCity::operator /=(size_t i) {}

    void OverrideCity::operator %(size_t i) {}
    void OverrideCity::operator %=(size_t i) {}

    void OverrideCity::operator ^(size_t i) {}
    void OverrideCity::operator ^=(size_t i) {}

    void OverrideCity::operator &(size_t i) {}
    void OverrideCity::operator &=(size_t i) {}
    void OverrideCity::operator &&(size_t i) {}

    void OverrideCity::operator |(size_t i) {}
    void OverrideCity::operator |=(size_t i) {}
    void OverrideCity::operator ||(size_t i) {}

    void OverrideCity::operator !(size_t i) {}
    void OverrideCity::operator !=(size_t i) {}

    void OverrideCity::operator ~(size_t i) {}

    void OverrideCity::operator =(size_t i) {}
    void OverrideCity::operator ==(size_t i) {}

    void OverrideCity::operator <(size_t i) {}
    void OverrideCity::operator <=(size_t i) {}
    void OverrideCity::operator <<(size_t i) {}
    void OverrideCity::operator <<=(size_t i) {}

    void OverrideCity::operator >(size_t i) {}
    void OverrideCity::operator >=(size_t i) {}
    void OverrideCity::operator >>(size_t i) {}
    void OverrideCity::operator >>=(size_t i) {}

    void OverrideCity::operator ,(size_t i) {}

    void OverrideCity::operator ->*(size_t i) {}
    void OverrideCity::operator ->(size_t i) {}


    void OverrideCity::operator (  )(size_t i) {}
    void OverrideCity::operator [ /*foobar*/  ](size_t i) {}

    OverrideCity& OverrideCity::operator/ /* awkwardly placed comment */ () {
        return *this;
    }

    // Test the type conversion operator.
    OverrideCity::operator bool() const {
        return false;
    }
    /* explicit */ OverrideCity::operator int*() const {
        return nullptr;
    }

    // Test C++11 user-defined literals.
    double OverrideCity::operator "" _foo(const char* str) {
        return some_conversion_of_str_to_double(str);
    }
