#include "http_request.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include "utils.h"

#define BUFFER_SIZE 65536

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

HTTPRequest::HTTPRequest() {
    this->cursor = 0;
}

HTTPRequestHeader::HTTPRequestHeader() {

}

ssize_t HTTPRequestHeader::parse(const char *buffer, size_t size) {
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
        size = read(fd, this->buffer + this->cursor, (size_t)(BUFFER_SIZE - this->cursor));
        if (size > 0) {
            this->cursor += size;
        } else if (errno == EAGAIN) {
            break;
        } else {
            IF_NEGATIVE_EXIT(-1);
        }
    }
    // try to parse header
    int end, content_len;
    try {
//        end = this->header.parse(this->buffer, 0, this->cursor);
        // content_len = string_to_int(this->header.header["Content-Length"]);
//        this->get_data(this->buffer, content_len, end, this->cursor);
    } catch (std::string err) {
    }
}
