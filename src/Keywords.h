#ifndef __KEYWORDS_H__
#define __KEYWORDS_H__

#include "StringView.h"

static struct {
    // Returns true if the following function must remain
    // entirely in the header.
    bool isHeaderOnly(const StringView& keyword) {
        return keyword == "inline"
            || keyword == "__force_inline";
    }

    // Returns true if the keyword cannot exist in the
    // definition of a function/object.
    bool isStrippedFromDefinition(const StringView& keyword) {
        return keyword == "virtual"
            || keyword == "explicit"
            || keyword == "static"
            || keyword == "override";
    }
} Keywords;

#endif //__KEYWORDS_H__
