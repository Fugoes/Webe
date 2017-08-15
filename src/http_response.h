#ifndef WEBE_HTTP_RESPONSE_H
#define WEBE_HTTP_RESPONSE_H

#include <string>
#include <tuple>
#include <list>

class HTTPResponse {
public:
    std::string version;
    std::string status;
    std::list<std::tuple<std::string, std::string>> header;
    std::string data;
    std::string raw;

    HTTPResponse();

    void parse();

    void send(int fd);

    static std::string date();
};

#endif //WEBE_HTTP_RESPONSE_H
