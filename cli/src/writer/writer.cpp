#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdint>

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
    lseek(this->fd, 0, SEEK_SET);
    for (size_t i = 0; write(this->fd, this->buf, this->buf_size) >= 0; ++i) {
        if (this->verbose) {
            std::cout << "Writing Block " << i << std::endl;
        }
    }
}

void DiskDestroyer::Writer::wr(char *config) {
    size_t cnt = this->buf_size / *(unsigned char*)config;
    size_t size = *(unsigned char*)config * cnt;
    char *iter = this->buf;
    for (size_t it = 0; it < cnt; ++it, iter += *(unsigned char*)config) {
        memcpy(iter, config + 1, *(unsigned char*)config);
    }
    lseek(this->fd, 0, SEEK_SET);
    for (size_t i = 0; write(this->fd, this->buf, size) >= 0; ++i) {
        if (this->verbose) {
            std::cout << "Writing Block " << i << std::endl;
        }
    }
}

void DiskDestroyer::Writer::operator()(char *config) {
    size_t round = 0;
    for (char *iter = config; *iter; ++iter, ++round) {
        if (this->verbose) {
            std::cout << "Round " << round << std::endl;
        }
        if (*iter == -1) {
            if (this->verbose) {
                std::cout << "RANDOM" << std::endl;
            }
            this->wr();
        } else {
            if (this->verbose) {
                std::cout << "PATTERN" << std::endl;
                for (
                    unsigned char i = 0, *ii = (unsigned char*)iter + 1;
                    i < *(unsigned char*)iter;
                    ++i, ++ii) {
                    snprintf(this->buf, 3, "%02hhX", *ii);
                    std::cout << buf;
                }
                std::cout << std::endl;
            }
            this->wr(iter);
            iter += *(unsigned char*)iter;
        }
    }
}
