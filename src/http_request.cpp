#include "http_request.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include "utils.h"

size_t get_key(const char *buffer, size_t begin, size_t end) {
    for (auto i = begin; i < end; i++) {
        if (buffer[i] == ':' && buffer[i + 1] == ' ') {
            return i;
        }
    }
    throw std::string("SyntaxError");
}

size_t get_word(const char *buffer, size_t begin, size_t end) {
    for (auto i = begin; i < end; i++) {
        if (buffer[i] == ' ') {
            return i;
        }
    }
    throw std::string("SyntaxError");
}

size_t get_word_CRLF(const char *buffer, size_t begin, size_t end) {
    for (auto i = begin; i < end; i++) {
        if (buffer[i] == '\r' && buffer[i + 1] == '\n') {
            return i;
        }
    }
    throw std::string("SyntaxError");
}

size_t string_to_size_t(std::string str) {
    size_t result;
    std::stringstream ss;
    ss << str;
    ss >> result;
    return result;
}

HTTPRequest::HTTPRequest() {
    this->cursor = 0;
    this->content = nullptr;
}

HTTPRequestHeader::HTTPRequestHeader() {

}

size_t HTTPRequestHeader::parse(const char *buffer, size_t size) {
    // GET /index.html HTTP/1.1
    size_t begin, end;
    // method
    begin = 0;
    end = get_word(buffer, 0, 7 < size ? 7 : size);
    this->method.assign(buffer + begin, end - begin);
    // uri
    begin = end + 1;
    end = get_word(buffer, begin, size);
    this->uri.assign(buffer + begin, end - begin);
    // version
    begin = end + 1;
    end = get_word_CRLF(buffer, begin, size);
    this->version.assign(buffer + begin, end - begin);
    // header
    begin = end + 2;
    if (begin + 1 >= size) throw std::string("IncompleteHeader");
    while (!(buffer[begin] == '\r' && buffer[begin + 1] == '\n')) {
        // Key: Value
        // Key
        end = get_key(buffer, begin, size);
        auto key = std::string(buffer + begin, end - begin);
        // Value
        begin = end + 2;
        end = get_word_CRLF(buffer, begin, size);
        auto value = std::string(buffer + begin, end - begin);
        this->header[key] = value;
        begin = end + 2;
        if (begin + 1 >= size) throw std::string("IncompleteHeader");
    }
    return begin + 2;
}

void HTTPRequest::parse(int fd) {
    ssize_t size;
    // Read until EAGAIN
    for (;;) {
        size = read(fd, this->buffer + this->cursor, buffer_size - this->cursor);
        if (size > 0) {
            this->cursor += size;
        } else if (errno == EAGAIN) {
            break;
        } else {
            IF_NEGATIVE_EXIT(-1);
        }
    }
    // try to parse header
}

void HTTPRequest::get_content() {
    size_t begin = this->header.parse(this->buffer, this->cursor);
    if (this->header.header.find("Content-Length") != this->header.header.end()) {
        // Found
        this->content_length = string_to_size_t(this->header.header["Content-Length"]);
        this->content = new char[this->content_length];
        if (begin == this->cursor) {
            this->content_written = 0;
            this->cursor = 0;
            return;
        } else if (begin < this->cursor) {
            this->content_written = this->cursor - begin;
            memcpy(this->content, this->buffer + begin, this->content_written);
            this->cursor = 0;
            return;
        } else {
            IF_NEGATIVE_EXIT(-1);
        }
    } else {
        // Not Found
        this->content = nullptr;
        return;
    }
}
