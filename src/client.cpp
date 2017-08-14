#include <iostream>
#include <sys/epoll.h>
#include "client.h"
#include "http_response.h"
#include "utils.h"

extern "C" {
#include <unistd.h>
#include <errno.h>
}

int Client::client_new = 0;
int Client::client_delete = 0;

Client::Client(int fd, std::string addr, uint16_t port_no, Server *server) : request(fd) {
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
    self->request.buffer.do_read();
    while (self->request.parse() == PARSE_NEW_REQUEST) {
        /*
        std::cout << "--->" << std::endl;
        std::cout << self->request.str() << std::endl;
        std::cout << "<---" << std::endl;
         */
        self->request.clean();
    }
}
