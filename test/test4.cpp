#include "../src/http_request.h"
#include <iostream>

int main() {
    char s[] = "GET / HTTP/1.1";
    HTTPRequest hr(0);
    hr.parse_request_line(std::make_tuple(s, sizeof(s) - 1));
    std::cout << hr.method << std::endl;
    std::cout << hr.uri << std::endl;
    std::cout << hr.version << std::endl;

    return 0;
}

