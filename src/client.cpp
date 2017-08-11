#include <iostream>
#include "client.h"

extern "C" {
#include <unistd.h>
#include <errno.h>
}

#define BUFFER_SIZE 65536

Client::Client(int fd, std::string addr, uint16_t port_no) {
    this->fd = fd;
    this->addr = addr;
    this->port_no = port_no;
    printf("New Client %s:%u\n", addr.c_str(), port_no);
}

void Client::handle_in(Client *self) {
    ssize_t size;
    for (;;) {
        size = read(self->fd, self->buffer, CLIENT_BUFFER_SIZE - 1);
        if (size > 0) {
            self->buffer[size] = '\0';
            printf("--> %d\n" "%s" "<--\n", self->fd, self->buffer);
        } else if (errno == EAGAIN) {
            break;
        }
    }
}
