#ifndef __test_cases_initializers_cch__
#define __test_cases_initializers_cch__

class test {
    int a;
    int x;
    
    test(int b);

    inline test()
        : a(0), x(0) {
    }
};

#endif
