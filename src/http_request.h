#ifndef WEBE_HTTP_REQUEST_H
#define WEBE_HTTP_REQUEST_H

#include <map>
#include <string>

const int buffer_size = 65536;

class HTTPRequestBuffer {
public:
    HTTPRequestBuffer(int fd);
    std::tuple<const char *, size_t> get_line();
    std::tuple<const char *, size_t> get(size_t size);
    void do_read();
    void do_flush();
    void do_clean();

    char buffer[buffer_size];
    // [left, right) are unused buffer
    size_t left;
    size_t right;
    int fd;
};

class HTTPRequest {
public:
    std::string method;
    std::string uri;
    std::string version;
    std::map<std::string, std::string> header;
    char *content;
    size_t content_length;

    HTTPRequest(int fd);

    HTTPRequestBuffer buffer;
};


#endif //WEBE_HTTP_REQUEST_H
