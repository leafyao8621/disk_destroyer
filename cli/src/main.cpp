#include <iostream>

#include "config/config.h"
#include "config/rc_config.h"
#include "config/parser.h"
#include "writer/writer.h"

const static char *msg =
    "Usage: dide [OPTIONS] <DEVICE>\n"
    "Arguments:\n"
    "<DEVICE>\n"
    "Options:\n"
    "-b <BLOCK SIZE>\n"
    "-p <PATTERN>\n"
    "    Acceptable values:\n"
    "        itsg, ict, hmg_baseline, hmg_enhanced, guttman,\n"
    "        {RC pattern name}, {pattern}\n"
    "-v";

int main(int argc, char **argv) {
    if (!argc) {
        std::cout << msg << std::endl;
        return 0;
    }
    size_t buf_size = 4096;
    bool verbose = false;
    std::string pattern = "itsg";
    int ret = 0;
    for (; (ret = getopt(argc, argv, "b:p:v")) != -1;) {
        switch (ret) {
        case 'b':
            buf_size = atol(optarg);
            break;
        case 'p':
            pattern = optarg;
            break;
        case 'v':
            verbose = true;
            break;
        case ':':
            std::cerr << msg << std::endl;
            return -1;
        case '?':
            std::cerr << msg << std::endl;
            return -1;
        }
    }
    if (optind != argc - 1) {
        std::cerr << msg << std::endl;
        return -1;
    }
    std::vector<char> pattern_compiled_raw;
    char *pattern_compiled = nullptr;
    DiskDestroyer::Config::RCConfig rc_config;
    if (pattern == "itsg") {
        pattern_compiled = (char*)DiskDestroyer::Config::itsg;
    } else if (pattern == "ict") {
        pattern_compiled = (char*)DiskDestroyer::Config::ict;
    } else if (pattern == "hmg_baseline") {
        pattern_compiled = (char*)DiskDestroyer::Config::hmg_baseline;
    } else if (pattern == "hmg_enhanced") {
        pattern_compiled = (char*)DiskDestroyer::Config::hmg_enhanced;
    } else if (pattern == "guttman") {
        pattern_compiled = (char*)DiskDestroyer::Config::guttman;
    } else {
        try {
            pattern_compiled = rc_config[pattern];
        } catch (DiskDestroyer::Config::RCConfig::Err) {
            try {
                pattern_compiled_raw =
                    DiskDestroyer::Config::Parser::parse(
                        (char*)pattern.c_str());
                if (
                    !DiskDestroyer::Config::Parser::validate(
                        pattern_compiled_raw)) {
                    std::cerr << msg << std::endl;
                    return -1;
                }
                pattern_compiled = pattern_compiled_raw.data();
            } catch (DiskDestroyer::Config::Parser::Err) {
                std::cerr << msg << std::endl;
                return -1;
            }
        }
    }
    if (verbose) {
        std::cout << "PATTERN" << std::endl;
        DiskDestroyer::Config::Parser::log(pattern_compiled, std::cout);
    }
    try {
        DiskDestroyer::Writer writer(argv[optind], buf_size, verbose);
        writer.init();
        writer(pattern_compiled);
    } catch (DiskDestroyer::Gen::Err err) {
        switch (err) {
        case DiskDestroyer::Gen::Err::OPEN:
            std::cerr << "GEN OPEN" << std::endl;
            return -1;
        case DiskDestroyer::Gen::Err::READ:
            std::cerr << "GEN READ" << std::endl;
            return -1;
        }
    } catch (DiskDestroyer::Writer::Err err) {
        switch (err) {
        case DiskDestroyer::Writer::Err::OPEN:
            std::cerr << "WRITER OPEN" << std::endl;
            return -1;
        case DiskDestroyer::Writer::Err::MEM:
            std::cerr << "WRITER MEM" << std::endl;
            return -1;
        }
    }
}
