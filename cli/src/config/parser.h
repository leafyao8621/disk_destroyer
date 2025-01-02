#ifndef CONFIG_PARSER_H_
#define CONFIG_PARSER_H_

#include <vector>
#include <iostream>

namespace DiskDestroyer {
    namespace Config {
        namespace Parser {
            enum Err {
                MALFORMED_STRING
            };
            std::vector<char> parse(char *config);
            bool validate(std::vector<char> &config);
            void log(char *config, std::ostream &os);
        }
    }
}

#endif
