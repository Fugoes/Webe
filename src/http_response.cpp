#include "http_response.h"
#include "utils.h"
#include <iostream>
#include <unistd.h>

HTTPResponse::HTTPResponse() { }

void HTTPResponse::parse() {
    this->raw = this->version + " " + this->status + "\r\n";
    for (auto && item : this->header) {
        this->raw += std::get<0>(item) + ": " + std::get<1>(item) + "\r\n";
    }
    this->raw += "\r\n";
    this->raw += this->data;
}

void HTTPResponse::send(int fd) {
    IF_NEGATIVE_EXIT(write(fd, this->raw.c_str(), this->raw.length()));
}

std::string HTTPResponse::date() {
    char buf[128];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    return std::string(buf);
}
