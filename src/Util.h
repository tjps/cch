#ifndef __UTIL_H__
#define __UTIL_H__

#include "StringView.h"

namespace Util {

    // Diff two strings, skipping any full '#line' directives
    // as long as they occur on the same line in both strings.
    bool diff(StringView a, StringView b);

    // Read the contents of filename.
    // Returns true on success, false if there was a read failure.
    bool readFromFile(const string& filename,
                      string* contents);

    // Given an output format string and the original input
    // filename, generate the expanded output path.
    // Returns true on success, setting outputPath to the expansion.
    // Returns false on failure, setting outputPath to an error message.
    bool expandOutputPath(const string& outputFormat,
                          const string& filename,
                          string* outputPath);
}

#endif //__UTIL_H__
