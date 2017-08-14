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
            if (this->parse_request_line(str, size) != 0) throw "ErrorParseRequestLine";
            this->status = WAITING_HEADER;
        case WAITING_HEADER:
            IF_FALSE_EXIT(this->status == WAITING_HEADER);
            while (1) {
                std::tie(str, size) = this->buffer.get_line();
                if (str == nullptr) return PARSE_FAILED;
                if (size == 0) {
                    this->status = WAITING_CONTENT;
                    break;
                }
                if (this->parse_header_line(str, size) != 0) throw "ErrorParseHeader";
            }
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

int HTTPRequest::parse_header_line(const char *str, ssize_t size) {
    ssize_t i = size;
    for (i = 0; i + 1 < size; i++) {
        if (str[i] == ':' && str[i + 1] == ' ') {
            break;
        }
    }
    if (i + 1 == size) {
        return -1;
    } else {
        this->header[std::string(str, i * sizeof(char))] = std::string(str + i + 2, (size - i - 2) * sizeof(char));
        return 0;
    }
}
