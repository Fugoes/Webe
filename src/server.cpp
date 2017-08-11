#include "server.h"
#include <cstring>
#include <iostream>

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
}

Server::Server(const char *server_addr, uint16_t port_no) {
    this->server_addr = std::string(server_addr);
    this->port_no = port_no;
}

Server::Server(std::string server_addr, uint16_t port_no) {
    this->server_addr = server_addr;
    this->port_no = port_no;
}

void Server::start() {
    this->do_socket();
    this->do_bind();
    this->do_listen();
    this->do_epoll();
    printf("Listen %s:%u\n", this->server_addr.c_str(), this->port_no);
    this->event_loop();
}

void Server::do_socket() {
    struct sockaddr_in server_sockaddr;
    this->server_sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    IF_NEGATIVE_EXIT(this->server_sock);
}

void Server::do_bind() {
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(this->port_no);
    serv_addr.sin_addr.s_addr = inet_addr(this->server_addr.c_str());
    IF_NEGATIVE_EXIT(bind(this->server_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)));
}

void Server::do_listen() {
    listen(this->server_sock, 5);
}

void Server::do_epoll() {
    this->epoll_fd = epoll_create1(0);
    IF_NEGATIVE_EXIT(this->epoll_fd);
}

void Server::event_loop(int max_events) {
    struct epoll_event ev, events[max_events];
    ev.events = EPOLLIN;
    ev.data.fd = this->server_sock;
    IF_NEGATIVE_EXIT(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, this->server_sock, &ev));

    int nevent, i, j;
    int client_sock;
    char ip_str[INET6_ADDRSTRLEN];
    struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof(struct sockaddr_in);
    for (;;) {
        nevent = epoll_wait(this->epoll_fd, events, max_events, -1);
        IF_NEGATIVE_EXIT(nevent);
        for (i = 0; i < nevent; i++) {
            if (events[i].data.fd == this->server_sock) {
                client_addrlen = sizeof(struct sockaddr_in);
                // new client
                client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_addrlen);
                IF_NEGATIVE_EXIT(client_sock);
                Server::set_nonblocking(client_sock);
                ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
                ev.data.fd = client_sock;
                IF_NEGATIVE_EXIT(epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, client_sock, &ev));
                inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));
                this->fd_to_client[client_sock] = new Client(client_sock, std::string(ip_str), client_addr.sin_port);
            } else {
                switch (events[i].events) {
                    case EPOLLIN:
                        printf("EPOLLIN\n");
                        this->fd_to_client[events[i].data.fd]->handle_in();
                        break;
                    case EPOLLRDHUP:
                    case EPOLLIN | EPOLLRDHUP:
                        printf("EPOLLRDHUP\n");
                        break;
                    default:
                        printf("Errno = %d\n", events[i].events);
                        IF_NEGATIVE_EXIT(-1);
                }
            }
        }
    }
}

void Server::set_nonblocking(int fd) {
    auto flag = fcntl(fd, F_GETFL, 0);
    flag |= O_NONBLOCK;
    IF_NEGATIVE_EXIT(fcntl(fd, F_SETFL, flag));
}
