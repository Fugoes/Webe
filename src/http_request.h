#ifndef WEBE_HTTP_REQUEST_H
#define WEBE_HTTP_REQUEST_H

#include <map>
#include <string>

const int buffer_size = 65536;

class HTTPRequestBuffer {
public:
    HTTPRequestBuffer(int fd);
    void do_read();
    std::tuple<const char *, size_t> get_word();
    std::tuple<const char *, size_t> get_till_CRLF();
    std::tuple<const char *, size_t> get_line();

// private:
    char buffer[buffer_size];
    // [left, right) are unused buffer
    size_t left;
    size_t right;
    int fd;
};

class HTTPRequestHeader {
public:
    HTTPRequestHeader();

    std::string method;
    std::string uri;
    std::string version;
    std::map<std::string, std::string> header;

    size_t parse(const char *buffer, size_t size);
};

class HTTPRequest {
public:
    HTTPRequest();

    void parse(int fd);

// private:
    HTTPRequestHeader header;
    char *content;
    size_t content_length;

    bool waiting_header;

    size_t content_written;

    char buffer[buffer_size];
    size_t cursor;
};


#endif //WEBE_HTTP_REQUEST_H
