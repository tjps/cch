#ifndef __STRINGVIEW_H__
#define __STRINGVIEW_H__

#include <string>
#include <string.h>  // for strlen()

// Provides an immutable view on a span of memory.
// The standard use case is as an accessor to slices
// of a backing string without needing to do any string
// creation.
//
class StringView {
    const char* mData;
    size_t mSize;

    StringView(const char* data, size_t size)
        : mData(data), mSize(size) {
    }

public:
    StringView(const char* str)
        : mData(str), mSize(strlen(str)) {
    }

    StringView(const string& str)
        : mData(str.data()), mSize(str.size()) {
    }

    // Return a subslice of this view.
    StringView slice(size_t start, size_t end) const {
        assert(start <= end);
        assert(end <= mSize);
        return StringView(mData + start, end - start);
    }

    bool operator==(const char* str) const {
        size_t i = 0;
        while (*str != '\0' && i < mSize && mData[i] == *str) {
            str++;
            i++;
        }
        return (*str == '\0' && i == mSize);
    }

    bool operator!=(const char* str) const {
        return !(*this == str);
    }

    const char* data() const {
        return mData;
    }

    size_t size() const {
        return mSize;
    }

    const char& operator[](size_t i) const {
        return mData[i];
    }

    string toString() const {
        return string(mData, mData + mSize);
    }
};

// Allow for writing a StringView directly to an ostream.
ostream& operator<< (ostream& out, const StringView& s) {
    out.write(s.data(), s.size());
    return out;
}

#endif //__STRINGVIEW_H__
