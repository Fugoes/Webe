#ifndef WEBE_PACKET_H
#define WEBE_PACKET_H

#include <string>
#include <vector>
#include <tuple>
#include <list>

using namespace std;

class HTTPHeader;
class HTTPResponse;

class HTTPHeader {
public:
    HTTPHeader();

    void set_status(string status_code);

    void append(string name, string value);

    string raw;
    static string date();

private:
    string status_code;
    list <tuple<string, string>> header;

    void parse();

    friend class HTTPResponse;
};


class HTTPResponse {
public:
    HTTPHeader header;
    string data;
    string raw;

    HTTPResponse();
    void parse();
    void send(int fd);
};

#endif //WEBE_PACKET_H
