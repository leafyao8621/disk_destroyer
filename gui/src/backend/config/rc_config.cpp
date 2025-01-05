#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "rc_config.h"
#include "parser.h"

DiskDestroyer::Config::RCConfig::RCConfig() {
    std::ostringstream oss;
    oss << getenv("HOME") << "/.diderc";
    std::ifstream ifs(oss.str().c_str());
    std::string key, value_raw;
    for (; ifs >> key >> value_raw;) {
        try {
            std::vector<char> value = Parser::parse((char*)value_raw.c_str());
            if (!Parser::validate(value)) {
                continue;
            }
            this->lookup[key] = value;
        } catch (Parser::Err) {
            continue;
        }
    }
}

char *DiskDestroyer::Config::RCConfig::operator[](std::string key) {
    if (this->lookup.find(key) == this->lookup.end()) {
        throw Err::NOT_FOUND;
    }
    return this->lookup[key].data();
}
