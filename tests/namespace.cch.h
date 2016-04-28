#ifndef __tests_namespace_cch__
#define __tests_namespace_cch__

using namespace std;

namespace ns {
    class a;
    class b {};
    class c {
    public:
        c();
        ~c();
    };
}

namespace /* anonymous */ {
    class no_one_can_hear_me_scream_in_here;
}
#endif
