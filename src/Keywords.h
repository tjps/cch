#ifndef __KEYWORDS_H__
#define __KEYWORDS_H__

#include "StringView.h"

static struct {
    bool isHeaderOnly(const StringView& keyword) {
        return keyword == "inline" || keyword == "__force_inline";
    }

    bool isStrippedFromDefinition(const StringView& keyword) {
        return keyword == "virtual" || keyword == "explicit" || keyword == "static";
    }
} Keywords;

#endif //__KEYWORDS_H__
