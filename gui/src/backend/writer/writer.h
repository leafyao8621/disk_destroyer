#ifndef BACKEND_WRITER_WRITER_H_
#define BACKEND_WRITER_WRITER_H_

#include <string>

#include "../gen/gen.h"

namespace DiskDestroyer {
    class Writer {
        std::string file_name;
        int fd;
        size_t buf_size;
        char *buf;
        Gen gen;
        bool verbose;
    public:
        enum Err {
            OPEN,
            MEM
        };
        Writer(std::string file_name, size_t buf_size, bool verbose);
        ~Writer();
        void init();
    private:
        void wr();
        void wr(char *config);
    public:
        void operator()(char *config);
    };
}

#endif
