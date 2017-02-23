#include <iostream>
#include <assert.h>
#include "Util.h"


int main(int argc, char** argv) {

    {
        assert(Util::diff("abcde\n12345\nxyz",
                          "abcde\n12345\nxyzw"));
        assert(Util::diff("abcde\n#lin\nxyz",
                          "abcde\n#lin\nxyzw"));
        assert(!Util::diff("abcde\n#line foo\nxyz",
                           "abcde\n#line bar\nxyz"));
    }

    {
        string output;
        bool expanded = Util::expandOutputPath("build/", "foobar", &output);
        assert(expanded);
        assert(output == "build/");
        expanded = Util::expandOutputPath("", "ignored", &output);
        assert(expanded);
        assert(output == "");
        expanded = Util::expandOutputPath("build/%p", "foo/test.cch", &output);
        assert(expanded);
        assert(output == "build/foo/test.cch");
        expanded = Util::expandOutputPath("build/%d/%f", "foo/test.cch", &output);
        assert(expanded);
        assert(output == "build/foo/test.cch");
        expanded = Util::expandOutputPath("build/%f", "foo/test.cch", &output);
        assert(expanded);
        assert(output == "build/test.cch");
        expanded = Util::expandOutputPath("build/%d/bar/%f", "foo/test.cch", &output);
        assert(expanded);
        assert(output == "build/foo/bar/test.cch");
        expanded = Util::expandOutputPath("%d%f%%%d%f", "foo/test.cch", &output);
        assert(expanded);
        assert(output == "footest.cch%footest.cch");

        expanded = Util::expandOutputPath("%", "filename", &output);
        assert(!expanded);
        expanded = Util::expandOutputPath("%d%X", "filename", &output);
        assert(!expanded);
    }
}
