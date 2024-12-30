#include <iostream>

#include "config/config.h"
#include "writer/writer.h"

int main(int argc, char **argv) {
    if (!argc) {
        std::cout << "USAGE" << std::endl;
        return 0;
    }
    size_t buf_size = 4096;
    bool verbose = false;
    verbose = true;
    try {
        DiskDestroyer::Writer writer(argv[1], buf_size, verbose);
        writer.init();
        writer((char*)DiskDestroyer::Config::ict);
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
