#include "read_buffer.h"
#include "utils.h"

extern "C" {
#include <string.h>
#include <unistd.h>
}

ReadBuffer::ReadBuffer(int fd) {
    this->fd = fd;
    this->left = 0;
    this->right = 0;
    this->buffer_size = 4;
    this->buffer = new char[this->buffer_size];
}

ReadBuffer::~ReadBuffer() {
    delete this->buffer;
}

void ReadBuffer::do_move() {
    IF_FALSE_EXIT(this->left <= this->right);
    if (this->left == this->right) {
        this->left = 0;
        this->right = 0;
        return;
    }
    if (this->left == 0) {
        return;
    }
    memmove(this->buffer, this->buffer + this->left, (this->right - this->left) * sizeof(char));
    this->right -= this->left;
    this->left = 0;
}

int ReadBuffer::do_read() {
    ssize_t size;
    this->do_move();
    auto old_right = this->right;
    IF_FALSE_EXIT(this->left == 0);
    while (1) {
        size = read(this->fd, this->buffer + this->right, (this->buffer_size - this->right) * sizeof(char));
        if (size > 0) {
            this->right += size;
            if (this->right == this->buffer_size) {
                // buffer full
                auto old_buffer = this->buffer;
                this->buffer = new char[this->buffer_size * 2];
                memcpy(this->buffer, old_buffer, this->buffer_size * sizeof(char));
                this->buffer_size *= 2;
                delete old_buffer;
                continue;
            }
        } else if (size == 0 || (size == -1 && errno == EAGAIN)) {
            if (this->right > old_right) {
                return 0;
            } else {
                return -1;
            }
        } else {
            IF_FALSE_EXIT(false);
        }
    }
}

std::tuple<const char *, ssize_t> ReadBuffer::get_line() {
    ssize_t i;
    for (i = this->left; i + 1 < this->right; i++) {
        if (this->buffer[i] == '\r' && this->buffer[i + 1] == '\n') {
            auto result = std::make_tuple(this->buffer + this->left, i - this->left);
            this->left = i + 2;
            return result;
        }
    }
    return std::make_tuple(nullptr, 0);
}

std::tuple<const char *, ssize_t> ReadBuffer::get_chars(ssize_t n) {
    if (this->right == this->left) {
        return std::make_tuple(nullptr, 0);
    }
    if (this->right - this->left >= n) {
        auto result = std::make_tuple(this->buffer + this->left, n);
        this->left += n;
        return result;
    } else {
        auto result = std::make_tuple(this->buffer + this->left, this->right - this->left);
        this->left = 0;
        this->right = 0;
        return result;
    }
}

void ReadBuffer::do_flush() {
    this->left = 0;
    this->right = 0;
}
