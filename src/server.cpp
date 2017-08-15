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
#include <sys/un.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
}

Server *this_server;

Server::Server(std::string server_addr, uint16_t port_no, uint64_t time_out /* = 60 */ ) {
    this->server_addr = server_addr;
    this->port_no = port_no;
    this->time_out = time_out;
    this->loaded_modules.clear();
    this->time_stamp = 0;
    this->udp_fd = -1;
}

void Server::start(std::vector<std::string> modules) {
    this->do_socket();
    this->do_bind();
    this->do_listen();
    this->do_epoll();
    this->do_timer();
    for (auto &&module : modules) {
        this->load_module(module);
    }
    this->do_udp();
    printf("Listen %s:%u\n", this->server_addr.c_str(), this->port_no);
    this->event_loop(1024);
}

void Server::do_socket() {
    int enable = 1;
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

void Server::event_loop(int max_events) {
    struct epoll_event ev, events[max_events];
    ev.events = EPOLLIN;
    ev.data.fd = this->server_sock;
    IF_NEGATIVE_EXIT(epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->server_sock, &ev));

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = this->timer_fd;
    IF_NEGATIVE_EXIT(epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->timer_fd, &ev));

    signal(SIGUSR1, Server::signal_handler);
    signal(SIGINT, Server::signal_handler);

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = this->udp_fd;
    IF_NEGATIVE_EXIT(epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->udp_fd, &ev));

    int nevent, i;
    int client_sock;
    uint64_t timer_buf;
    char ip_str[INET6_ADDRSTRLEN];
    struct sockaddr_in client_addr;
    socklen_t client_addrlen;
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
                for (auto func : this->timer_hook) {
                    func(this);
                }
                if (this->time_stamp % 10 == 0) {
                    // clean dead connection
                    this->clean_old_connections();
                }
            } else if (events[i].data.fd == this->udp_fd) {
                // udp
                this->command_handler();
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
    if (this->time_stamp <= this->time_out)
        return;
    auto boundary = this->time_stamp - this->time_out;
    Client *c;
    for (auto iter = this->fd_to_client.begin(); iter != this->fd_to_client.end();) {
        c = std::get<1>(*iter);
        if (c->time_stamp < boundary) {
            iter = this->fd_to_client.erase(iter);
            delete c;
        } else ++iter;
    }
}

void Server::load_module(std::string module) {
    if (available_modules.find(module) != available_modules.end()) {
        auto handle = dlopen(("libmodule_" + module + ".so").c_str(), RTLD_LAZY);
        this->loaded_modules[module] = handle;
        for (auto && i : this->loaded_modules) {
            std::cout << std::get<0>(i) << ": " << std::get<1>(i);
        }
        auto module_load = (ModuleLoad) dlsym(handle, "module_load");
        module_load(this);
    }
}

void Server::do_udp() {
    this->udp_fd = socket(AF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    IF_NEGATIVE_EXIT(this->udp_fd);

    struct sockaddr_un name;

    name.sun_family = AF_UNIX;
    strcpy(name.sun_path, ("/tmp/webed_" + std::to_string(getpid()) + ".sock").c_str());
    IF_NEGATIVE_EXIT(bind(this->udp_fd, (struct sockaddr *) &name, sizeof(struct sockaddr_un)));
}

void Server::signal_handler(int dunno) {
    switch (dunno) {
        case SIGUSR1:
            printf("Alive %d\t\tNew %d\t\tDelete %d\n", Client::client_new - Client::client_delete,
                   Client::client_new, Client::client_delete);
            break;
        case SIGINT:
            unlink(("/tmp/webed_" + std::to_string(getpid()) + ".sock").c_str());
            exit(0);
        default:
            IF_NEGATIVE_EXIT(-1);
    }
}

void Server::command_handler() {
    char buf[1024];
    ssize_t size = read(this->udp_fd, buf, sizeof(buf));
    std::string cmd;
    if (size > 0) {
        cmd.assign(buf, size * sizeof(char));
    } else {
        IF_FALSE_EXIT(false);
    }
    std::cout << cmd.substr(12) << std::endl;
    if (cmd.substr(0, 11) == "load-module") {
        this->load_module(cmd.substr(12));
    }
    if (cmd.substr(0, 13) == "unload-module") {
        this->unload_module(cmd.substr(14));
    }
}

void Server::unload_module(std::string module) {
    auto m = this->loaded_modules.find(module);
    if (m != this->loaded_modules.end()) {
        auto handle = std::get<1>(*m);
        auto module_unload = (ModuleUnload) dlsym(handle, "module_unload");
        module_unload(this);
        std::cout << "Unload Module!" << std::endl;
    } else {
        std::cout << "Module " << module << " is not loaded!\n";
    }
}
