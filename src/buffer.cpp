#include "buffer.h"
#include "utils.h"

extern "C" {
#include <string.h>
#include <unistd.h>
}

Buffer::Buffer(int fd) {
    this->fd = fd;
    this->left = 0;
    this->right = 0;
}

std::tuple<const char *, ssize_t> Buffer::get_line() {
    ssize_t i;
    for (i = this->left; i + 1 < this->right; i++) {
        if (this->buf[i] == '\r' && this->buf[i + 1] == '\n') {
            auto result = std::make_tuple(this->buf + this->left, i - this->left);
            this->left = i + 2;
            return result;
        }
    }
    return std::make_pair(nullptr, 0);
}

int Buffer::do_read() {
    IF_FALSE_EXIT(left <= right);
    ssize_t size;
    while (1) {
        if (this->right == buffer_size) {
            return EBUSY;
        }
        size = read(this->fd, this->buf + this->right, (buffer_size - this->right) * sizeof(char));
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

void Buffer::do_move() {
    IF_FALSE_EXIT(left <= right);
    if (this->left == this->right) {
        this->left = 0;
        this->right = 0;
        return;
    }
    if (this->left == 0) {
        return;
    }
    memmove(this->buf, this->buf + this->left, (this->right - this->left) * sizeof(char));
    this->right -= this->left;
    this->left = 0;
}

void Buffer::do_flush() {
    this->left = 0;
    this->right = 0;
    return;
}

std::tuple<const char *, ssize_t> Buffer::get_chars(ssize_t n) {
    IF_FALSE_EXIT(n > 0);
    if (this->right == this->left) {
        return std::make_tuple(nullptr, 0);
    }
    if (this->right - this->left <= n) {
        auto result = std::make_tuple(this->buf + this->left, this->right - this->left);
        this->left = 0;
        this->right = 0;
        return result;
    } else {
        auto result = std::make_tuple(this->buf + this->left, n);
        this->left += n;
        return result;
    }
}

