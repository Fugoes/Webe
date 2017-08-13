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
    throw GET_LINE_FAILED;
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

size_t HTTPRequestBuffer::get_word(const char *s, size_t left, size_t right) {
    for (size_t i = left; i < right; i++) {
        if (s[i] == ' ') {
            return i;
        }
    }
    return 0;
}

size_t HTTPRequestBuffer::get_key(const char *s, size_t left, size_t right) {
    for (size_t i = left; i < right - 1; i++) {
        if (s[i] == ':' && s[i + 1] == ' ') {
            return i;
        }
    }
    return 0;
}

HTTPRequest::HTTPRequest(int fd) : buffer(fd) {
    this->status = WAITING_REQUEST_LINE;
    this->content = nullptr;
}

bool HTTPRequest::parse() {
    const char *begin;
    size_t size;
    size_t left, right;
    switch (this->status) {
        case WAITING_REQUEST_LINE:
            try {
                std::tie(begin, size) = this->buffer.get_line();
            } catch (HTTPRequestError e) {
                switch (e) {
                    case GET_LINE_FAILED:
                        return false;
                    default:
                        IF_FALSE_EXIT(false);
                }
            }
            left = 0;
            right = HTTPRequestBuffer::get_word(begin, 0, size);
            if (right != 0) {
                this->method.assign(begin + left, right - left);
            } else throw INVALID_REQUEST_LINE;
            left = right + 1;
            right = HTTPRequestBuffer::get_word(begin, left, size);
            if (right != 0) {
                this->uri.assign(begin + left, right - left);
            } else throw INVALID_REQUEST_LINE;
            left = right + 1;
            right = size;
            if (left >= right) throw INVALID_REQUEST_LINE;
            this->version.assign(begin + left, right - left);
            this->status = WAITING_HEADER;
        case WAITING_HEADER:
            while (1) {
                try {
                    std::tie(begin, size) = this->buffer.get_line();
                } catch (HTTPRequestError e) {
                    switch (e) {
                        case GET_LINE_FAILED:
                            return false;
                        default:
                            IF_FALSE_EXIT(false);
                    }
                }
                if (size != 0) {
                    left = 0;
                    right = HTTPRequestBuffer::get_key(begin, left, size);
                    if (right != 0) {
                        this->header[std::string(begin + left, right - left)]
                                = std::string(begin + right + 2, size - right - 2);
                    } else throw INVALID_HEADER;
                } else {
                    if (this->header.find("Content-Length") != this->header.end()) {
                        // Found
                        this->content_length = string_to_size_t(this->header["Content-Length"]);
                        this->content_get = 0;
                        this->new_content(this->content_length);
                        break;
                    } else {
                        // Not Found
                        this->content = nullptr;
                        this->status = WAITING_REQUEST_LINE;
                        return true;
                    }
                }
            }
            this->status = WAITING_CONTENT;
        case WAITING_CONTENT:
            IF_FALSE_EXIT(this->content_get <= this->content_length);
            std::tie(begin, size) = this->buffer.get(this->content_length - this->content_get);
            if (size != 0) {
                memcpy(this->content + this->content_get, begin, size);
            }
            this->content_get += size;
            if (this->content_get == this->content_length)
                this->status = WAITING_REQUEST_LINE;
            return true;
            break;
        default:
            IF_FALSE_EXIT(false);
    }
}

void HTTPRequest::new_content(size_t size) {
    if (this->content != nullptr) {
        delete this->content;
    }
    this->content = new char[size];
}

std::string HTTPRequest::str() {
    std::string result("");
    result += this->method + " " + this->uri + " " + this->version + "\n";
    for (auto &&i : this->header) {
        result += i.first + ": " + i.second + "\n";
    }
    result += "\n";
    result += std::string(this->content, this->content_length);
    return result;
}

HTTPRequest::~HTTPRequest() {
    if (this->content != nullptr) {
        delete this->content;
    }
}
