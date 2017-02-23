#include "StringView.h"

ostream& operator<< (ostream& out, const StringView& s) {
    out.write(s.data(), s.size());
    return out;
}
