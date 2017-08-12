#include <iostream>
#include "client.h"
#include "packet.h"
#include "utils.h"

extern "C" {
#include <unistd.h>
#include <errno.h>
}

#define BUFFER_SIZE 65536

int Client::client_new = 0;
int Client::client_delete = 0;

Client::Client(int fd, std::string addr, uint16_t port_no) {
    this->fd = fd;
    this->addr = addr;
    this->port_no = port_no;
    // printf("New Client %s:%u\n", addr.c_str(), port_no);
    Client::client_new++;
}

void Client::handle_in(Client *self) {
    ssize_t size;
    for (;;) {
        size = read(self->fd, self->buffer, CLIENT_BUFFER_SIZE - 1);
        if (size > 0) {
            HTTPResponse r;
            r.data = "Hello World\r\n";
            r.header.set_status("200 OK");
            r.header.append("Server", "Webe/0.1");
            r.header.append("Date", HTTPHeader::date());
            r.header.append("Content-Type", "text/html");
            r.header.append("Connection", "Keep-Alive");
            r.parse();
            r.send(self->fd);
        } else if (errno == EAGAIN) {
            break;
        } else {
            IF_NEGATIVE_EXIT(-1);
        }
    }
}

void Client::handle_rdhup(Client *self) {
    // printf("Client %s:%u disconnect\n", self->addr.c_str(), self->port_no);
    delete self;
}

Client::~Client() {
    close(this->fd);
    Client::client_delete++;
}
