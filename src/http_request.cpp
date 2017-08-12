#include "http_request.h"
#include <unistd.h>
#include <iostream>
#include <cerrno>
#include "utils.h"

#define BUFFER_SIZE 65536

// [begin, end)
int get_key(const char buf[], int begin, int end) {
    int i;
    for (i = begin; i < end; i++) {
        if (buf[i] == ':' && buf[i + 1] == ' ') {
            return i;
        }
    }
    throw "SyntaxError";
}

// [begin, end)
int get_word(const char buf[], int begin, int end) {
    int i;
    for (i = begin; i < end; i++) {
        if (buf[i] == ' ') {
            return i;
        }
    }
    throw "SyntaxError";
}

// [begin, end)
int get_word_CRLF(const char buf[], int begin, int end) {
    int i;
    for (i = begin; i < end; i++) {
        if (buf[i] == '\r' && buf[i + 1] == '\n') {
            return i;
        }
    }
    throw "SyntaxError";
}

void HTTPRequest::try_parse(int fd) {
    static ssize_t size = 0;
    // read until cannot read
    for (;;) {
        size = read(fd, this->buf + this->cursor, BUFFER_SIZE - this->cursor);
        if (size > 0) {
            this->cursor += size;
        } else if (errno == EAGAIN) {
            break;
        } else {
            IF_NEGATIVE_EXIT(-1);
        }
    }
    // parse buf
}

HTTPRequest::HTTPRequest() {
    this->cursor = 0;
}

HTTPRequestHeader::HTTPRequestHeader() {

}

int HTTPRequestHeader::parse(char *buf, int left, int right) {
    // GET /index.html HTTP/1.1
    // GET
    int begin, end;
    begin = left;
    end = get_word(buf, begin, begin + 7);
    buf[end] = '\0';
    this->method = std::string(buf + begin);
    buf[end] = ' ';
    // /index.html
    begin = end + 1;
    end = get_word(buf, begin, right);
    buf[end] = '\0';
    this->uri = std::string(buf + begin);
    buf[end] = ' ';
    // HTTP/1.1
    begin = end + 1;
    end = get_word_CRLF(buf, begin, right);
    buf[end] = '\0';
    this->version = std::string(buf + begin);
    buf[end] = '\r';
    begin = end + 2;
    // Parse key -> value pair
    while (!(buf[begin] == '\r' && buf[begin + 1] == '\n')) {
        // Key: Value
        // Key
        end = get_key(buf, begin, right);
        buf[end] = '\0';
        auto key = std::string(buf + begin);
        buf[end] = ':';
        // Value
        begin = end + 2;
        end = get_word_CRLF(buf, begin, right);
        buf[end] = '\0';
        auto value = std::string(buf + begin);
        buf[end] = '\r';
        begin = end + 2;
        // Add (Key, Value) to header
        this->header[key] = value;
    }
    return begin + 2;
}
