#ifndef BACKEND_CONFIG_RC_CONFIG_H_
#define BACKEND_CONFIG_RC_CONFIG_H_

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
            auto begin() { return this->lookup.begin(); };
            auto begin() const { return this->lookup.begin(); };
            auto end(){ return this->lookup.end(); };
            auto end() const { return this->lookup.end(); };
        };
    }
}

#endif
