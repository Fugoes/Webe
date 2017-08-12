#include "http_response.h"
#include "utils.h"
#include <sstream>
#include <iostream>
#include <unistd.h>

HTTPResponse::HTTPResponse() {

}

void HTTPResponse::parse() {
    std::stringstream ss;
    std::string content_length;
    ss << this->data.length();
    ss >> content_length;
    this->header.append("Content-Length", content_length);
    this->header.parse();
    this->raw = this->header.raw + this->data;
}

void HTTPResponse::send(int fd) {
    IF_NEGATIVE_EXIT(write(fd, this->raw.c_str(), this->raw.length()));
}

HTTPResponseHeader::HTTPResponseHeader() {}

void HTTPResponseHeader::append(std::string name, std::string value) {
    this->header.push_back(std::make_tuple(name, value));
}

void HTTPResponseHeader::set_status(std::string status_code) {
    this->status_code = status_code;
}

void HTTPResponseHeader::parse() {
    this->raw = "HTTP/1.1 " + this->status_code + "\r\n";
    for (auto &&item : this->header) {
        this->raw += std::get<0>(item) + ": " + std::get<1>(item) + "\r\n";
    }
    this->raw += "\r\n";
}

std::string HTTPResponseHeader::date() {
    char buf[128];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    return std::string(buf);
}
