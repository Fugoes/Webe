#include "http_request.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
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
    throw std::string("SyntaxError");
}

// [begin, end)
int get_word(const char buf[], int begin, int end) {
    int i;
    for (i = begin; i < end; i++) {
        if (buf[i] == ' ') {
            return i;
        }
    }
    throw std::string("SyntaxError");
}

// [begin, end)
int get_word_CRLF(const char buf[], int begin, int end) {
    int i;
    for (i = begin; i < end; i++) {
        if (buf[i] == '\r' && buf[i + 1] == '\n') {
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

int HTTPRequestHeader::parse(const char *buf, int left, int right) {
    // GET /index.html HTTP/1.1
    // GET
    int begin, end;
    begin = left;
    end = get_word(buf, begin, begin + 7);
    this->method.assign(buf + begin, (unsigned long)(end - begin));
    // /index.html
    begin = end + 1;
    end = get_word(buf, begin, right);
    this->uri.assign(buf + begin, (unsigned long)(end - begin));
    // HTTP/1.1
    begin = end + 1;
    end = get_word_CRLF(buf, begin, right);
    this->version.assign(buf + begin, (unsigned long)(end - begin));
    begin = end + 2;
    // Parse key -> value pair
    if (!(begin + 1 < right)) throw std::string("SyntaxError");
    while (!(buf[begin] == '\r' && buf[begin + 1] == '\n')) {
        // Key: Value
        // Key
        end = get_key(buf, begin, right);
        auto key = std::string(buf + begin, (unsigned long)(end - begin));
        // Value
        begin = end + 2;
        end = get_word_CRLF(buf, begin, right);
        auto value = std::string(buf + begin, (unsigned long)(end - begin));
        // Add (Key, Value) to header
        this->header[key] = value;
        begin = end + 2;
        if (!(begin + 1 < right)) throw std::string("SyntaxError");
    }
    return begin + 2;
}

void HTTPRequest::parse(int fd) {
    ssize_t size;
    // Read until EAGAIN
    for (;;) {
        size = read(fd, this->buf + this->cursor, (size_t)(BUFFER_SIZE - this->cursor));
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
        end = this->header.parse(this->buf, 0, this->cursor);
        // content_len = string_to_int(this->header.header["Content-Length"]);
        this->get_data(this->buf, content_len, end, this->cursor);
    } catch (std::string err) {
    }
}

void HTTPRequest::get_data(char *buf, int content_len, int left, int right) {
    if (content_len + left <= right) {
        // full content has been received
        char dummy = this->buf[content_len + left];
        this->buf[content_len + left] = '\0';
        this->data = this->buf + left;
        this->buf[content_len + left] = dummy;
        if (content_len + left == this->cursor) {
            this->cursor = 0;
        } else {
            memmove(this->buf, this->buf + left, (size_t)content_len);
        }
    }
}
