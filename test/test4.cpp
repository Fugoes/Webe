#include <iostream>
#include "../src/http_request.h"

int main() {
    HTTPRequest hr(0);
    char str[] = "GET /index.html HTTP/1.1";
    ssize_t size = sizeof(str) - 1;
    hr.parse_request_line(str, size);
    std::cout << hr.method << "\n";
    std::cout << hr.uri << "\n";
    std::cout << hr.version << "\n";
    char stra[] = "Content-Length: sdfjlkadsf";
    char strb[] = "Content-Lengbh: sdfjlkadsf";
    hr.parse_header_line(stra, sizeof(stra) - 1);
    hr.parse_header_line(strb, sizeof(strb) - 1);
    for (auto && i : hr.header) {
        std::cout << std::get<0>(i) << ":" << std::get<1>(i) << std::endl;
    }
    return 0;
}