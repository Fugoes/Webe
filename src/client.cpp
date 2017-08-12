#include <iostream>
#include <sys/epoll.h>
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

Client::Client(int fd, std::string addr, uint16_t port_no, Server *server) {
    this->fd = fd;
    this->addr = addr;
    this->port_no = port_no;
    this->server = server;
    Client::client_new++;
}

void Client::handle_rdhup(Client *self) {
    delete self;
}

Client::~Client() {
    epoll_ctl(this->server->epoll_fd, EPOLL_CTL_DEL, this->fd, NULL);
    this->server->fd_to_client.erase(this->fd);
    close(this->fd);
    Client::client_delete++;
}

void Client::handle_in(Client *self) {
    self->time_stamp = self->server->time_stamp;
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
