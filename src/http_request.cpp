#include "http_request.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include "utils.h"

size_t string_to_size_t(std::string str) {
    size_t result;
    std::stringstream ss;
    ss << str;
    ss >> result;
    return result;
}

HTTPRequest::HTTPRequest(int fd) : buffer(fd) {
    this->content = nullptr;
    this->status = WAITING_REQUEST_LINE;
}

HTTPRequest::~HTTPRequest() {
    if (this->content != nullptr) {
        delete this->content;
    }
}

ParseStatus HTTPRequest::parse() {

}
