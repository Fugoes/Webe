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
        this->content = nullptr;
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
                if (size != 0) {
                    if (this->parse_header_line(str, size) != 0) throw "ErrorParseHeader";
                } else {
                    if (this->header.find("Content-Length") != this->header.end()) {
                        this->content_length = string_to_size_t(this->header["Content-Length"]);
                        if (this->content != nullptr) {
                            delete this->content;
                            this->content = nullptr;
                        }
                        this->content = new char[this->content_length];
                        this->content_received = 0;
                    } else {
                        this->content_length = 0;
                    }
                    this->status = WAITING_CONTENT;
                    break;
                }
            }
        case WAITING_CONTENT:
            IF_FALSE_EXIT(this->status == WAITING_CONTENT);
            if (this->content_length == 0 || this->content_length == this->content_received) {
                this->status = GET_A_REQUEST;
            } else {
                std::tie(str, size) = this->buffer.get_chars(this->content_length - this->content_received);
                if (str == nullptr) return PARSE_FAILED;
                memcpy(this->content + this->content_received, str, size * sizeof(char));
                this->content_received += size;
                if (this->content_received == this->content_length) {
                    this->status = GET_A_REQUEST;
                } else {
                    return PARSE_FAILED;
                }
            }
        case GET_A_REQUEST:
            IF_FALSE_EXIT(this->status == GET_A_REQUEST);
            return PARSE_NEW_REQUEST;
        default:
            IF_FALSE_EXIT(false);
    }
}

int HTTPRequest::parse_request_line(const char *str, ssize_t size) {
    // TODO
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

std::string HTTPRequest::str() {
    auto result = this->method + " " + this->uri + " " + this->version + "\n";
    for (auto && i : this->header) {
        result += std::get<0>(i) + ": " + std::get<1>(i) + "\n";
    }
    if (this->content_length > 0) {
        result += std::string(this->content, this->content_length * sizeof(char));
    }
    return result;
}

void HTTPRequest::clean() {
    this->status = WAITING_REQUEST_LINE;
    this->header.clear();
    if (this->content != nullptr) {
        delete this->content;
        this->content = nullptr;
        this->content_length = 0;
        this->content_received = 0;
    }
}
