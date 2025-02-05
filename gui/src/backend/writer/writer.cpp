#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdint>

#include <sys/ioctl.h>
#include <linux/fs.h>

#include <gtk-3.0/gtk/gtk.h>
#include "writer.h"

DiskDestroyer::Writer::Writer(
    std::string file_name,
    size_t buf_size,
    GDiskDestroyer::App *app) {
    this->file_name = file_name;
    this->fd = -1;
    this->buf_size = buf_size;
    this->buf = nullptr;
    this->app = app;
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
    if ((this->fd = open(this->file_name.c_str(), O_RDWR | O_SYNC)) == -1) {
        throw Err::OPEN;
    }
    size_t sz = 0;
    ioctl(fd, BLKGETSIZE64, &sz);
    if (!sz) {
        throw Err::OPEN;
    }
    this->cutoff = sz / this->buf_size / 100;
    if (!(this->buf = new char[this->buf_size])) {
        throw Err::MEM;
    }
    this->gen.init();
}

int append_log(gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->append_log();
    return 0;
}

void DiskDestroyer::Writer::wr() {
    this->gen(this->buf_size, this->buf);
    lseek(this->fd, 0, SEEK_SET);
    for (
        size_t i = 0, j = 0;
        write(this->fd, this->buf, this->buf_size) >= 0;
        ++j) {
        if (this->app->get_stop()) {
            return;
        }
        if (!(j % this->cutoff)) {
            std::ostringstream oss;
            oss << "Written " << ++i << "%" << std::endl;
            this->app->set_message(oss.str());
            g_idle_add(append_log, this->app);
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
    for (
        size_t i = 0, j = 0;
        write(this->fd, this->buf, this->buf_size) >= 0;
        ++j) {
        if (this->app->get_stop()) {
            return;
        }
        if (!(j % this->cutoff)) {
            std::ostringstream oss;
            oss << "Written " << ++i << "%" << std::endl;
            this->app->set_message(oss.str());
            g_idle_add(append_log, this->app);
        }
    }
}

void DiskDestroyer::Writer::operator()(char *config) {
    size_t round = 0;
    for (char *iter = config; *iter; ++iter, ++round) {
        if (this->app->get_stop()) {
            return;
        }
        std::ostringstream oss;
        oss << "Round " << round << std::endl;
        if (*iter == -1) {
            oss << "RANDOM" << std::endl;
            this->app->set_message(oss.str());
            g_idle_add(append_log, this->app);
            this->wr();
        } else {
            oss << "PATTERN" << std::endl;
            for (
                unsigned char i = 0, *ii = (unsigned char*)iter + 1;
                i < *(unsigned char*)iter;
                ++i, ++ii) {
                snprintf(this->buf, 3, "%02hhX", *ii);
                oss << buf;
            }
            oss << std::endl;
            this->app->set_message(oss.str());
            g_idle_add(append_log, this->app);
            this->wr(iter);
            iter += *(unsigned char*)iter;
        }
    }
}
