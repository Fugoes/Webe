#include <iostream>
#include "http_request.h"
#include "server.h"

extern "C" {
#include <dlfcn.h>
}

int main(int argc, char *argv[]) {
    uint16_t port_no = 8080;
    std::string address = "0.0.0.0";
    for (auto i = 1; i < argc;) {
        if (std::string(argv[i]) == "-p") {
            i++;
            port_no = (uint16_t) atoi(argv[i]);
            i++;
        } else if (std::string(argv[i]) == "-a") {
            i++;
            address = std::string(argv[i]);
            i++;
        } else {
            IF_NEGATIVE_EXIT(-1);
        }
    }
    auto server = Server(address, port_no, 1200);
    server.start();
    return 0;
}