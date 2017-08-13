#include "server.h"
#include <cstring>
#include <cinttypes>
#include <iostream>
#include <forward_list>
#include <signal.h>

extern "C" {
#include <sys/types.h>
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
}

Server::Server(std::string server_addr, uint16_t port_no, uint64_t time_out /* = 60 */ ) {
    this->server_addr = server_addr;
    this->port_no = port_no;
    this->time_out = time_out;
}

void Server::start() {
    this->do_socket();
    this->do_bind();
    this->do_listen();
    this->do_epoll();
    this->do_timer();
    printf("Listen %s:%u\n", this->server_addr.c_str(), this->port_no);
    this->event_loop(1024);
}

void Server::do_socket() {
    int enable = 1;
    struct sockaddr_in server_sockaddr;
    this->server_sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    IF_NEGATIVE_EXIT(this->server_sock);
    IF_NEGATIVE_EXIT(setsockopt(this->server_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)));
    IF_NEGATIVE_EXIT(setsockopt(this->server_sock, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)));
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

void Server::do_timer() {
    this->timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    IF_NEGATIVE_EXIT(this->timer_fd);
    struct itimerspec it;
    IF_NEGATIVE_EXIT(clock_gettime(CLOCK_MONOTONIC, &it.it_value));
    it.it_value.tv_sec = 1;
    it.it_value.tv_nsec = 0;
    it.it_interval.tv_sec = 1;
    it.it_interval.tv_nsec = 0;
    timerfd_settime(this->timer_fd, 0, &it, NULL);
}

void handle_sig(int dunno) {
    switch (dunno) {
        case SIGUSR1:
            printf("Alive %d\t\tNew %d\t\tDelete %d\n", Client::client_new - Client::client_delete,
                   Client::client_new, Client::client_delete);
            break;
        default:
            IF_NEGATIVE_EXIT(-1);
    }
}

void Server::event_loop(int max_events) {
    struct epoll_event ev, events[max_events];
    ev.events = EPOLLIN;
    ev.data.fd = this->server_sock;
    IF_NEGATIVE_EXIT(epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->server_sock, &ev));

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = this->timer_fd;
    IF_NEGATIVE_EXIT(epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->timer_fd, &ev));

    signal(SIGUSR1, handle_sig);

    int nevent, i, j = 0;
    int client_sock;
    uint64_t timer_buf;
    char ip_str[INET6_ADDRSTRLEN];
    struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof(struct sockaddr_in);
    // event loop
    for (;;) {
        nevent = epoll_wait(this->epoll_fd, events, max_events, -1);
        // std::cout << nevent << std::endl;
        if (nevent == -1 && errno == EINTR) {
            continue;
        } else {
            IF_NEGATIVE_EXIT(nevent);
        }
        for (i = 0; i < nevent; i++) {
            if (events[i].data.fd == this->server_sock) { // new client
                client_addrlen = sizeof(struct sockaddr_in);
                client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_addrlen);
                IF_NEGATIVE_EXIT(client_sock);
                Server::set_nonblocking(client_sock);
                ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
                ev.data.fd = client_sock;
                IF_NEGATIVE_EXIT(epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, client_sock, &ev));
                inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));
                this->fd_to_client[client_sock] = new Client(client_sock, std::string(ip_str), client_addr.sin_port,
                                                             this);
                this->fd_to_client[client_sock]->time_stamp = this->time_stamp;
            } else if (events[i].data.fd == this->timer_fd) {
                // timer
                IF_NEGATIVE_EXIT(read(this->timer_fd, &timer_buf, 8));
                this->time_stamp += timer_buf;
                printf("| Alive %8d | New %8d | Delete %8d |\n", Client::client_new - Client::client_delete,
                       Client::client_new, Client::client_delete);
                if (this->time_stamp % 5 == 0) {
                    // clean dead connection
                    this->clean_old_connections();
                }
            } else {
                switch (events[i].events) {
                    case EPOLLIN: // ready for read
                        Client::handle_in(this->fd_to_client[events[i].data.fd]);
                        break;
                    default:
                        Client::handle_rdhup(this->fd_to_client[events[i].data.fd]);
                        break;
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

void Server::clean_old_connections() {
    std::forward_list<std::unordered_map<int, Client *>::iterator> to_delete;
    std::cout << this->fd_to_client.size() << " ";
    auto boundary = this->time_stamp - 5;
    for (auto iter = this->fd_to_client.begin(); iter != this->fd_to_client.end();) {
        if (this->time_stamp <= this->time_out) break;
        if (std::get<1>(*iter)->time_stamp < boundary) {
            to_delete.push_front(iter);
        }
    }
    for (auto && s : to_delete) {
        auto tmp = std::get<1>(*s);
        this->fd_to_client.erase(s);
        delete tmp;
    }
    std::cout << this->fd_to_client.size() << "\n";
}
