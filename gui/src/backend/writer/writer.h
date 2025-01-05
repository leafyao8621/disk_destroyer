#ifndef BACKEND_WRITER_WRITER_H_
#define BACKEND_WRITER_WRITER_H_

#include <string>
#include <list>
#include <sstream>

#include "../gen/gen.h"
#include "../../frontend/app.h"

namespace DiskDestroyer {
    class Writer {
        std::string file_name;
        int fd;
        size_t buf_size;
        char *buf;
        Gen gen;
        GtkWindow *window;
        std::list<std::string> *messenger;
        std::ostringstream oss;
    public:
        enum Err {
            OPEN,
            MEM
        };
        Writer(
            std::string file_name,
            size_t buf_size,
            GtkWindow *window,
            std::list<std::string> *messenger);
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
