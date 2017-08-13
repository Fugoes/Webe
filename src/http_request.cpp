#include "http_request.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include "utils.h"

size_t string_to_size_t(std::string str) {
    size_t result;
    std::stringstream ss;
    ss << str;
    ss >> result;
    return result;
}

HTTPRequestBuffer::HTTPRequestBuffer(int fd) {
    this->fd = fd;
    this->right = 0;
    this->left = 0;
}

int HTTPRequestBuffer::do_read() {
    ssize_t size;
    this->do_flush();
    IF_FALSE_EXIT(this->left == 0);
    while (1) {
        IF_FALSE_EXIT(this->right <= buffer_size);
        if (this->right == buffer_size) {
            return EBUSY;
        }
        size = read(fd, this->buffer + this->left, buffer_size - this->right);
        if (size > 0) {
            this->right += size;
        } else if (errno == EAGAIN) {
            return EAGAIN;
        } else {
            IF_NEGATIVE_EXIT(-1);
        }
    }
    return 0;
}

std::tuple<const char *, size_t> HTTPRequestBuffer::get_line() {
    for (auto i = this->left; i < this->right - 1; i++) {
        if (this->buffer[i] == '\r' && this->buffer[i + 1] == '\n') {
            auto result = std::make_tuple(this->buffer + this->left, i - this->left);
            this->left = i + 2;
            return result;
        }
    }
    throw std::string("GetLine");
}

void HTTPRequestBuffer::do_flush() {
    if (this->left == 0) {
        return;
    }
    if (this->left == this->right) {
        this->left = 0;
        this->right = 0;
        return;
    }
    memmove(this->buffer, this->buffer + this->left, this->right - this->left);
}

void HTTPRequestBuffer::do_clean() {
    this->left = 0;
    this->right = 0;
}

std::tuple<const char *, size_t> HTTPRequestBuffer::get(size_t size) {
    IF_FALSE_EXIT(size > 0);
    if (this->right - this->left <= size) {
        auto result = std::make_tuple(this->buffer + this->left, this->right - this->left);
        this->left = 0;
        this->right = 0;
        return result;
    } else {
        auto result = std::make_tuple(this->buffer + this->left, size);
        this->left += size;
        return result;
    }
}

HTTPRequest::HTTPRequest(int fd) : buffer(fd) {

}


