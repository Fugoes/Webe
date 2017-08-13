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
    return std::make_pair(nullptr, 0);
}

int ReadBuffer::do_read() {
    IF_FALSE_EXIT(left <= right);
    ssize_t size;
    while (1) {
        if (this->right == buffer_size) {
            return EBUSY;
        }
        size = read(this->fd, this->buffer + this->right, (buffer_size - this->right) * sizeof(char));
        if (size > 0) {
            this->right += size;
        } else if (errno == EAGAIN) {
            return EAGAIN;
        } else if (size == 0) {
            return 0;
        } else {
            IF_FALSE_EXIT(false);
        }
    }
}

void ReadBuffer::do_move() {
    IF_FALSE_EXIT(left <= right);
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

void ReadBuffer::do_flush() {
    this->left = 0;
    this->right = 0;
    return;
}

std::tuple<const char *, ssize_t> ReadBuffer::get_chars(ssize_t n) {
    IF_FALSE_EXIT(n > 0);
    if (this->right == this->left) {
        return std::make_tuple(nullptr, 0);
    }
    if (this->right - this->left <= n) {
        auto result = std::make_tuple(this->buffer + this->left, this->right - this->left);
        this->left = 0;
        this->right = 0;
        return result;
    } else {
        auto result = std::make_tuple(this->buffer + this->left, n);
        this->left += n;
        return result;
    }
}

