#include <iostream>

#include "writer.h"

DiskDestroyer::Writer::Writer(
    std::string file_name, size_t buf_size, bool verbose) {
    this->file_name = file_name;
    this->fd = -1;
    this->buf_size = buf_size;
    this->buf = nullptr;
    this->verbose = verbose;
}

DiskDestroyer::Writer::~Writer() {
    if (this->fd != -1) {
        close(this->fd);
    }
    if (this->buf) {
        delete[] this->buf;
    }
    this->gen.init();
}

void DiskDestroyer::Writer::init() {
    if ((this->fd = open(this->file_name.c_str(), O_WRONLY)) == -1) {
        throw Err::OPEN;
    }
    if (!(this->buf = new char[this->buf_size])) {
        throw Err::MEM;
    }
    this->gen.init();
}

void DiskDestroyer::Writer::wr() {
    this->gen(this->buf_size, this->buf);
    for (size_t i = 0; write(this->fd, this->buf, this->buf_size) >= 0; ++i) {
        if (this->verbose) {
            std::cout << "Writing Block " << i << std::endl;
        }
    }
}

void DiskDestroyer::Writer::operator()(char *config) {
    size_t round = 0;
    for (char *iter = config; *iter; ++round) {
        if (this->verbose) {
            std::cout << "Round " << round << std::endl;
        }
        if (*iter == -1) {
            if (this->verbose) {
                std::cout << "RANDOM" << std::endl;
            }
            this->wr();
            ++iter;
        }
    }
}
