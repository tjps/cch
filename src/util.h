#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <fstream>
#include <sstream>

static string replaceAll(string str, const string& from, const string& to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

static void writeFile(const string& filename,
                      const string& banner,
                      const stringstream& content) {
    ofstream file(filename.c_str(), ios::binary);
    if (!banner.empty()) {
        file << banner << endl;
    }
    file << content.str();
    file.close();
}

#endif //__UTIL_H__
