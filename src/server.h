#ifndef WEBE_SERVER_H
#define WEBE_SERVER_H

#include "utils.h"
#include <string>
#include <unordered_map>
#include <map>
#include <cstdint>
#include <vector>
#include <tuple>

class Server;

#include "client.h"
#include "modules.h"


class Server {
public:
    Server(std::string server_addr, uint16_t port_no, uint64_t time_out = 60);

    void start();


private:
    std::string server_addr;
    uint16_t port_no;
    int server_sock;
    int epoll_fd;
    int timer_fd;
    uint64_t time_stamp;
    uint64_t time_out;
    std::unordered_map<int, Client *> fd_to_client;

    // name -> (handle, ModuleUnload)
    std::map<std::string, std::tuple<int, ModuleUnload>> loaded_modules;

    std::vector<HTTPRequestHandler> http_request_hook;
    std::vector<TimerHandler> timer_hook;

    void do_socket();

    void do_bind();

    void do_listen();

    void do_epoll();

    void do_timer();

    void event_loop(int max_events = 128);

    void clean_old_connections();

    static void set_nonblocking(int fd);

    friend class Client;
};


#endif //WEBE_SERVER_H
