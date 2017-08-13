#include <iostream>
#include "http_request.h"
#include <string>
#include <cstdint>
#include "server.h"

int main(int argc, char *argv[]) {
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
    HTTPRequest hr(0);
    for (auto i = 0; i < sizeof(s) - 1; i++) {
        hr.buffer.buffer[i] = s[i];
    }
    hr.buffer.left = 0;
    hr.buffer.right = sizeof(s) - 1;

    if (hr.parse()) {
        std::cout << "true\n";
        std::cout << hr.str();
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