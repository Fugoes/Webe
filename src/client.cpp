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

void handle_request(HTTPRequest *r) {
    std::cout << "--->" << std::endl;
    std::cout << r->str() << std::endl;
    std::cout << "<---" << std::endl;
}

void Client::handle_in(Client *self) {
    self->time_stamp = self->server->time_stamp;
    int flag;
    while (1) {
        std::cout << "Hi\n";
        flag = self->request.buffer.do_read();
        if (flag == EAGAIN) {
            break;
        } else if (flag == EBUSY) {
            while (self->request.parse()) {
                handle_request(&self->request);
            }
            self->request.buffer.do_flush();
        }
    }
    while (self->request.parse()) {
        handle_request(&self->request);
    }
}
