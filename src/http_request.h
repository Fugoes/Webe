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

    ssize_t parse(const char *buffer, size_t size);
};

class HTTPRequest {
public:
    HTTPRequest();

    void parse(int fd);

private:
    HTTPRequestHeader header;

    char buffer[buffer_size];
    int cursor;

    char *data;

    void get_data(char *buf, int content_len, int left, int right);
};


#endif //WEBE_HTTP_REQUEST_H
