#include "parser.h"

std::vector<char> DiskDestroyer::Config::Parser::parse(char *config) {
    std::vector<char> out;
    char cur = 0;
    bool high = true;
    for (char *iter = config; *iter; ++iter) {
        if (*iter >= '0' && *iter <= '9') {
            if (high) {
                cur = (*iter - '0') << 4;
            } else {
                cur |= *iter - '0';
                out.push_back(cur);
            }
        } else if (*iter >= 'A' && *iter <= 'F') {
            if (high) {
                cur = ((*iter - 'A') + 10) << 4;
            } else {
                cur |= (*iter - 'A') + 10;
                out.push_back(cur);
            }
        } else if (*iter >= 'a' && *iter <= 'f') {
            if (high) {
                cur = ((*iter - 'a') + 10) << 4;
            } else {
                cur |= (*iter - 'a') + 10;
                out.push_back(cur);
            }
        } else {
            throw Err::MALFORMED_STRING;
        }
        high = !high;
    }
    if (!high) {
        throw Err::MALFORMED_STRING;
    }
    out.push_back(0);
    return out;
}

bool DiskDestroyer::Config::Parser::validate(std::vector<char> &config) {
    char *iter = config.data();
    for (; iter < config.data() + config.size(); ++iter) {
        if (*iter == -1) {
            continue;
        } else if (*iter) {
            iter += *(unsigned char*)iter;
        } else {
            break;
        }
    }
    return iter == config.data() + config.size() - 1;
}

void DiskDestroyer::Config::Parser::log(char *config, std::ostream &os) {
    size_t round = 0;
    char buf[10];
    for (char *iter = config; *iter; ++iter, ++round) {
        os << "Round " << round << std::endl;
        if (*iter == -1) {
            os << "RANDOM" << std::endl;
        } else {
            os << "PATTERN" << std::endl;
            for (
                unsigned char i = 0, *ii = (unsigned char*)iter + 1;
                i < *(unsigned char*)iter;
                ++i, ++ii) {
                snprintf(buf, 3, "%02hhX", *ii);
                os << buf;
            }
            os << std::endl;
            iter += *(unsigned char*)iter;
        }
    }
}
