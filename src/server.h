#ifndef WEBE_SERVER_H
#define WEBE_SERVER_H

#include "utils.h"
#include <string>
#include <unordered_map>
#include <cstdint>

class Server;

#include "client.h"


class Server {
public:
    Server(const char *server_addr, uint16_t port_no);

    Server(std::string server_addr, uint16_t port_no);

    void start();

    static int time_stamp;

private:
    std::string server_addr;
    uint16_t port_no;
    int server_sock;
    int epoll_fd;
    int timer_fd;
    std::unordered_map<int, Client *> fd_to_client;

    void do_socket();

    void do_bind();

    void do_listen();

    void do_epoll();

    void do_timer();

    void event_loop(int max_events = 128);

    static void set_nonblocking(int fd);

    friend class Client;
};


#endif //WEBE_SERVER_H
