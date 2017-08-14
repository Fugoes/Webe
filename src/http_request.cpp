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
            while (1) {
                std::tie(str, size) = this->buffer.get_line();
                if (str == nullptr) {
                    if (size == 0) {
                    } else if (size == -1) {

                    } else {
                        IF_FALSE_EXIT(false);
                    }
                }
            }
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

bool HTTPRequest::parse_request_line(std::tuple<const char *, ssize_t> arg) {
    const char *str;
    ssize_t size;
    std::tie(str, size) = arg;
    ssize_t i;
    for (i = 0; i < size; i++) {
        if (str[i] == ' ') {
            this->method.assign(str, (size_t)i);
            break;
        }
    }
    if (i == size) throw PARSE_FAILED;
    str += i + 1;
    size -= i + 1;
    for (i = 0; i < size; i++) {
        if (str[i] == ' ') {
            this->uri.assign(str, (size_t)i);
            break;
        }
    }
    str += i + 1;
    size -= i + 1;
    this->version.assign(str, (size_t)size);
    return true;
}
