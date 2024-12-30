#include "gen.h"

DiskDestroyer::Gen::Gen() {
    this->fd = -1;
}

DiskDestroyer::Gen::~Gen() {
    if (this->fd == -1) {
        return;
    }
    close(this->fd);
}

void DiskDestroyer::Gen::init() {
    if ((this->fd = open("/dev/urandom", O_RDONLY)) == -1) {
        throw Err::OPEN;
    }
}

void DiskDestroyer::Gen::operator()(size_t size, char *buf) {
    if ((size_t)read(this->fd, buf, size) != size) {
        throw Err::READ;
    }
}
