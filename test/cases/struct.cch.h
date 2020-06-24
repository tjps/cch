#pragma once


struct named {
    struct /* unnamed type member */ {
        int x;
        union /* unnamed union member */ {
            long y;
            void* p;
        };
    } s;
    double d;
};

