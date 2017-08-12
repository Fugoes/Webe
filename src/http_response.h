#ifndef WEBE_HTTP_RESPONSE_H
#define WEBE_HTTP_RESPONSE_H

#include <string>
#include <tuple>
#include <list>

class HTTPResponseHeader;
class HTTPResponse;

class HTTPResponseHeader {
public:
    HTTPResponseHeader();

    void set_status(std::string status_code);

    void append(std::string name, std::string value);

    std::string raw;

    static std::string date();

private:
    std::string status_code;
    std::list <std::tuple<std::string, std::string>> header;

    void parse();

    friend class HTTPResponse;
};


class HTTPResponse {
public:
    HTTPResponseHeader header;
    std::string data;
    std::string raw;

    HTTPResponse();

    void parse();

    void send(int fd);
};

#endif //WEBE_HTTP_RESPONSE_H
