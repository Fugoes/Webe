#include "packet.h"
#include "utils.h"
#include <sstream>
#include <iostream>
#include <unistd.h>

HTTPResponse::HTTPResponse() {

}

void HTTPResponse::parse() {
    stringstream ss;
    string content_length;
    ss << this->data.length();
    ss >> content_length;
    this->header.append("Content-Length", content_length);
    this->header.parse();
    this->raw = this->header.raw + this->data;
}

void HTTPResponse::send(int fd) {
    IF_NEGATIVE_EXIT(write(fd, this->raw.c_str(), this->raw.length()));
}

HTTPHeader::HTTPHeader() { }

void HTTPHeader::append(string name, string value) {
    this->header.push_back(make_tuple(name, value));
}

void HTTPHeader::set_status(string status_code) {
    this->status_code = status_code;
}

void HTTPHeader::parse() {
    this->raw = "HTTP/1.1 " + this->status_code + "\r\n";
    for (auto &&item : this->header) {
        this->raw += get<0>(item) + ": " + get<1>(item) + "\r\n";
    }
    this->raw += "\r\n";
}

string HTTPHeader::date() {
    char buf[128];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    return string(buf);
}