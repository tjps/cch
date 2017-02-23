#include <fstream>
#include <iostream>
#include <libgen.h> // for dirname(), basename()
#include <sstream>
#include <string>
#include "Util.h"

bool Util::diff(StringView a, StringView b) {
    size_t end = 0;
    while (a.find('\n', &end)) {
        end += 1; // capture the '\n' too.
        StringView lineA = a.slice(0, end);
        StringView lineB = b.slice(0, min(end, b.size()));
        if (lineA.size() >= 5 && ::memcmp(lineA.data(), "#line", 5) == 0) {
            if (lineB.size() >= 5 && ::memcmp(lineB.data(), "#line", 5) != 0) {
                return true;
            }
        } else if (lineA != lineB) {
            return true;
        }
        a = a.slice(lineA.size(), a.size());
        b = b.slice(lineB.size(), b.size());
    }
    return a != b;
}

bool Util::readFromFile(const string& filename,
                        string* contents) {
    ifstream inputFile(filename.c_str(),
                       ios::in|ios::binary|ios::ate);
    if (!inputFile.good()) {
        return false;
    }
    contents->resize(inputFile.tellg());
    inputFile.seekg(0, ios::beg);
    inputFile.read(&(*contents)[0], contents->size());
    return true;
}

bool Util::expandOutputPath(const string& outputFormat,
                            const string& filename,
                            string* outputPath) {
    string expanded;
    expanded.reserve(outputFormat.size() + filename.size());
    // Note: the POSIX versions of basename/dirname modify the string
    //   in place and thus do not need to be/should not be free()'d.
    char scratch[filename.size() + 1];
    strcpy(scratch, filename.c_str());
    char* basename = ::basename(scratch);
    char* dirname = ::dirname(scratch);
    //cout << "dirname: " << dirname << ", basename: " << basename << endl;
    for (size_t i = 0; i < outputFormat.size(); i++) {
        if (outputFormat[i] == '%') {
            char specifier =  (i + 1 < outputFormat.size())
                ? outputFormat[i+1]
                : '\0';
            switch (specifier) {
            case '%': expanded += '%'; break;
            case 'p': expanded += filename; break;
            case 'd': expanded += dirname; break;
            case 'f': expanded += basename; break;
            default:
                *outputPath = "ERROR: invalid format string '" +
                    outputFormat + "'\n     invalid format specified " +
                    string(i+1, '-') + "^";
                return false;
            }
            i++;
        } else {
            expanded += outputFormat[i];
        }
    }
    outputPath->swap(expanded);
    return true;
}
