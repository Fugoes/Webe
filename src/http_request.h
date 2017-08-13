#ifndef WEBE_HTTP_REQUEST_H
#define WEBE_HTTP_REQUEST_H

#include <map>
#include <string>

const int buffer_size = 65536;

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
    void get_content();

// private:
    HTTPRequestHeader header;

    char buffer[buffer_size];
    size_t cursor;

    char *content;
    size_t content_written;
    size_t content_length;
};


#endif //WEBE_HTTP_REQUEST_H
