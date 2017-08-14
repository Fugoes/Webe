#ifndef WEBE_HTTP_REQUEST_H
#define WEBE_HTTP_REQUEST_H

#include "read_buffer.h"
#include <map>
#include <string>

enum ParseStatus {
    PARSE_FAILED,
    PARSE_NEW_REQUEST,
};

class HTTPRequest {
public:
    std::string method;
    std::string uri;
    std::string version;
    std::map<std::string, std::string> header;
    char *content;
    ssize_t content_length;

    HTTPRequest(int fd);

    ~HTTPRequest();

    /**
     * The request line and header's total length shall be smaller than buffer_size defined in read_buffer.h
     * @return
     * PARSE_FAILED,
     * PARSE_EAGAIN,
     * PARSE_NEW_REQUEST,
     */
    ParseStatus parse();

    int parse_request_line(const char *str, ssize_t size);

    int parse_header_line(const char *str, ssize_t size);

private:
    ssize_t content_received;
    ReadBuffer buffer;
    enum {
        WAITING_REQUEST_LINE,
        WAITING_HEADER,
        WAITING_CONTENT,
        GET_A_REQUEST,
    } status;
};


#endif //WEBE_HTTP_REQUEST_H
