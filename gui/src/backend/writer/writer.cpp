#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdint>

#include <gtk-3.0/gtk/gtk.h>
#include "writer.h"

DiskDestroyer::Writer::Writer(
    std::string file_name,
    size_t buf_size,
    GtkWindow *window,
    std::list<std::string> *messenger) {
    this->file_name = file_name;
    this->fd = -1;
    this->buf_size = buf_size;
    this->buf = nullptr;
    this->window = window;
    this->messenger = messenger;
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
        this->oss.clear();
        oss << "Writing Block " << i << std::endl;
        messenger->push_back(oss.str());
        g_signal_emit_by_name(this->window, "append-log");
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
        this->oss.clear();
        oss << "Writing Block " << i << std::endl;
        messenger->push_back(oss.str());
        g_signal_emit_by_name(this->window, "append-log");
    }
}

void DiskDestroyer::Writer::operator()(char *config) {
    size_t round = 0;
    for (char *iter = config; *iter; ++iter, ++round) {
        this->oss.clear();
        this->oss << "Round " << round << std::endl;
        if (*iter == -1) {
            this->oss << "RANDOM" << std::endl;
            messenger->push_back(oss.str());
            g_signal_emit_by_name(this->window, "append-log");
            this->wr();
        } else {
            this->oss << "PATTERN" << std::endl;
            for (
                unsigned char i = 0, *ii = (unsigned char*)iter + 1;
                i < *(unsigned char*)iter;
                ++i, ++ii) {
                snprintf(this->buf, 3, "%02hhX", *ii);
                this->oss << buf;
            }
            this->oss << std::endl;
            messenger->push_back(oss.str());
            g_signal_emit_by_name(this->window, "append-log");
            this->wr(iter);
            iter += *(unsigned char*)iter;
        }
    }
}
