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
    const char *str;
    ssize_t size;
    switch (this->status) {
        case WAITING_REQUEST_LINE:
            IF_FALSE_EXIT(this->status == WAITING_REQUEST_LINE);
            std::tie(str, size) = this->buffer.get_line();
            if (str == nullptr) return PARSE_FAILED;

        case WAITING_HEADER:
            IF_FALSE_EXIT(this->status == WAITING_HEADER);
        case WAITING_CONTENT:
            IF_FALSE_EXIT(this->status == WAITING_CONTENT);
        case GET_A_REQUEST:
            IF_FALSE_EXIT(this->status == GET_A_REQUEST);
        default:
            IF_FALSE_EXIT(false);
    }
}

int HTTPRequest::parse_request_line(const char *str, ssize_t size) {
    ssize_t i = size;
    for (i = 0; i < size; i++) {
        if (str[i] == ' ') {
            this->method.assign(str, i * sizeof(char));
            break;
        }
    }
    i++;
    str += i;
    size -= i;
    for (i = 0; i < size; i++) {
        if (str[i] == ' ') {
            this->uri.assign(str, i * sizeof(char));
            break;
        }
    }
    i++;
    str += i;
    size -= i;
    this->version.assign(str, size * sizeof(char));
    return 0;
}
