#include <iostream>
#include "http_request.h"
#include <string>
#include <cstdint>
#include "server.h"

int main(int argc, char *argv[]) {
    HTTPRequest r;
    HTTPRequestHeader b;
    char s[] = "GET / HTTP/1.1\r\n"
            "Host: test test\r\n"
            "User-Agent: python-requests/2.18.1\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "Connection: keep-alive\r\n"
            "Accept: */*\r\n"
            "Content-Length: 12\r\n"
            "BLa: sdfadfsadf\r\n"
            "\r\n"
            "Hello World!";
    for (int i = 0; i < sizeof(s); i++) {
        r.buffer[i] = s[i];
    }
    r.cursor = sizeof(s) - 1;
    r.get_content();
    for (int i = 0; i < r.content_length; i++) {
        putchar(r.content[i]);
    }
    std::cout << "$" << std::endl;
    b.parse(s, sizeof(s) - 1);
    std::cout << b.method << " " << b.uri << " " << b.version << std::endl;
    for (auto && item : b.header) {
        std::cout << item.first << ": " << item.second << std::endl;
    }
    if (b.header.find("Content-Length") == b.header.end()) {
        std::cout << "Not found\n";
    } else {
        std::cout << b.header["Content-Length"] << std::endl;
    }
    /*
    uint16_t port_no = 8080;
    std::string address = "0.0.0.0";
    for (auto i = 1; i < argc;) {
        if (std::string(argv[i]) == "-p") {
            i++;
            port_no = (uint16_t)atoi(argv[i]);
            i++;
        } else if (std::string(argv[i]) == "-a") {
            i++;
            address = std::string(argv[i]);
            i++;
        } else {
            IF_NEGATIVE_EXIT(-1);
        }
    }
    auto server = Server(address, port_no);
    server.start();
    return 0;
     */
}