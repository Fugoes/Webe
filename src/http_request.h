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

    int parse(char *buf, int left, int right);
};

class HTTPRequest {
public:
    HTTPRequest();

    void try_parse(int fd);

private:
    HTTPRequestHeader header;
    char buf[buffer_size];
    size_t cursor;
};


#endif //WEBE_HTTP_REQUEST_H
