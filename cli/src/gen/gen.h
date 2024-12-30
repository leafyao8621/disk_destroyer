#ifndef GEN_GEN_H_
#define GEN_GEN_H_

#include <unistd.h>
#include <fcntl.h>

namespace DiskDestroyer {
    class Gen {
        int fd;
    public:
        enum Err {
            OPEN,
            READ
        };
        Gen();
        ~Gen();
        void init();
        void operator()(size_t size, char *buf);
    };
}

#endif
