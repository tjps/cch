#pragma once


enum outerEnum { a, b, c, d, e };

class enumHolder {
public:
    enum innerEnum { w,
                     x,
                     y,
                     z };

    enum /* un-named */ {
        m, n, o, p
    };
};

