#ifndef WEBE_HTTP_REQUEST_H
#define WEBE_HTTP_REQUEST_H

#include <map>
#include <string>

const int buffer_size = 65536;

enum HTTPRequestError {
    GET_LINE_FAILED,
    INVALID_REQUEST_LINE,
    INVALID_HEADER
};

class HTTPRequestBuffer {
public:
    HTTPRequestBuffer(int fd);

    std::tuple<const char *, size_t> get_line();

    std::tuple<const char *, size_t> get(size_t size);

    int do_read();

    void do_flush();

    void do_clean();

    static size_t get_word(const char *s, size_t left, size_t right);

    static size_t get_key(const char *s, size_t left, size_t right);

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
    size_t content_get;
    size_t content_length;

    HTTPRequest(int fd);
    ~HTTPRequest();

    bool parse();

    void new_content(size_t size);

    std::string str();

    HTTPRequestBuffer buffer;
    enum {
        WAITING_REQUEST_LINE,
        WAITING_HEADER,
        WAITING_CONTENT,
    } status;
};


#endif //WEBE_HTTP_REQUEST_H
