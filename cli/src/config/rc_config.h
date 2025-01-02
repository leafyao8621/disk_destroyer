#ifndef CONFIG_RC_CONFIG_H_
#define CONFIG_RC_CONFIG_H_

#include <unordered_map>
#include <vector>
#include <string>

namespace DiskDestroyer {
    namespace Config {
        class RCConfig {
            std::unordered_map<std::string, std::vector<char> > lookup;
        public:
            enum Err {
                NOT_FOUND
            };
            RCConfig();
            char *operator[](std::string key);
        };
    }
}

#endif
