#include <iostream>
#include "../src/http_request.h"
#include <string>
#include <cstdint>
#include "../src/server.h"

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
}
