#include <assert.h> // for assert()
#include <getopt.h> // for getopt()
#include <stdlib.h> // for abort()
#include <fstream>
#include "Tokenizer.h"
#include "Parser.h"
#include "Util.h"

using namespace Util;

static void writeToFile(const string& filename,
                        const string& banner,
                        const stringstream& content,
                        bool diffAware) {
    string newContents = (!banner.empty() ? (banner + "\n") : "") + content.str();
    string existingContents;
    if (!diffAware
        || !readFromFile(filename, &existingContents)
        || diff(newContents, existingContents)) {
        ofstream file(filename.c_str(), ios::binary);
        file << newContents;
        file.close();
    } else {
        cerr << "Contents of " << filename << " unchanged, skipping writing" << endl;
    }
}

void version() {
    cerr << "CCH - " << Version::kRepoURL << endl <<
        "Version: " << Version::kBuildVersion << "" << endl;
}

namespace Defaults {
    static const char* ccExtension = "cc";
    static const char* hExtension = "h";
    static const char* outputFormat = "%p";
};

int main(int argc, char** argv) {
    string cchFilename;
    string outputFormat = Defaults::outputFormat;
    string ccExtension = Defaults::ccExtension;
    string hExtension = Defaults::hExtension;
    bool debug = false;
    bool includeBanner = true;
    bool emitLineNumbers = true;
    bool diffAware = false;
    bool usage = false;

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"debug", no_argument, 0, 'd'},
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"version", no_argument, 0, 'v'},
        {"noBanner", no_argument, 0, 1},
        {"noLineNumbers", no_argument, 0, 2},
        {"ccExtension", required_argument, 0, 3},
        {"hExtension", required_argument, 0, 4},
        {"diff", no_argument, 0, 5},
        {0, 0, 0, 0}
    };

    for (int c = 0, optindex = 0;
         (c = getopt_long(argc, argv, "hdi:o:v",
                          long_options, &optindex)) != -1; ) {
        switch (c) {
        case 1:   includeBanner = false; break;
        case 2:   emitLineNumbers = false; break;
        case 3:   ccExtension = optarg; break;
        case 4:   hExtension = optarg; break;
        case 5:   diffAware = true; break;
        case 'd': debug = true; break;
        case 'i': cchFilename = optarg; break;
        case 'o': outputFormat = optarg; break;
        case 'v': version(); return 1;
        case 'h':
        case '?':
        default:  usage = true; break;
        }
    }
    if (usage
        || (optind < argc)
        || cchFilename.empty()) {

        if (optind < argc) {
            cerr << "Unrecognized arguments:";
            for (int i = optind; i < argc; i++) {
                cerr << " " << argv[i];
            }
            cerr << endl << endl;
        }
        if (usage) {
            version();
        }
        cerr << "Usage: " << argv[0] << " [OPTIONS] -i/--input=<file> " <<
            " [-o/--output=<format string>]" << endl << endl <<
            "   Required:\n"
            "      -i <file>, --input=<file> Input CCH file\n"
            "   Optional:\n"
            "      -o <fmt>, --output=<fmt>  Output location format string (Default: \"" << Defaults::outputFormat << "\")\n"
            "      -d, --debug               Enable debug output\n"
            "      -h, --help                Show this help menu and exit\n"
            "      -v, --version             Show program version and exit\n"
            "      --noLineNumbers           Don't emit #line directives\n"
            "      --noBanner                Don't add CCH banner to generated files\n"
            "      --ccExtension=<ext>       Set output extension (Default: " << Defaults::ccExtension << ")\n"
            "      --hExtension=<ext>        Set output extension (Default: " << Defaults::hExtension << "\n"
            "   Experimental:    (**subject to change/removal**)\n"
            "      --diff                    Enable content-aware diff for not rewriting\n"
            "                                an output if no source change occurred for it\n";
        return 1;
    }

    // Populate cch with the contents of the .cch file.
    string cch;
    if (!readFromFile(cchFilename, &cch)) {
        cerr << "ERROR: failed to open input: " << cchFilename << endl;
        return 2;
    }

    stringstream cc, h;
    {   // Split cch into the cc and h buffers.
        ParseContext ctx(cchFilename, &cc, &h, emitLineNumbers);
        BaseTokenizer tokenizer;
        BaseParser parser(&ctx, &tokenizer);

        WrapperParser typeChanger(parser);
        tokenizer.tokenize(cch, &typeChanger);
    }

    (void)debug; // Suppress unused warning for now, until debug flag is used again.


    string baseOutputFilename;
    if (!expandOutputPath(outputFormat, cchFilename, &baseOutputFilename)) {
        cerr << baseOutputFilename << endl;
        return 1;
    }
    string ccFilename = baseOutputFilename + "." + ccExtension;
    string hFilename = baseOutputFilename + "." + hExtension;
    cout << "[CCH] " << cchFilename << " split to { " <<
        hFilename << ", " << ccFilename << " }" << endl;
    string banner;
    if (includeBanner) {
        banner = "// Generated by CCH (";
        banner += Version::kRepoURL;
        banner += ") ";
        banner += Version::kBuildVersion;
    }
    writeToFile(ccFilename, banner, cc, diffAware);
    writeToFile(hFilename, banner, h, diffAware);
    return 0;
}
