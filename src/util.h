#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>

static string replaceAll(string str, const string& from, const string& to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

#endif //__UTIL_H__
