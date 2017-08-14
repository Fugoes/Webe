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
    return 0;
}