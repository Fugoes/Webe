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

    void start(std::vector<std::string> modules);

    void load_module(std::string module);

    void unload_module(std::string module);

    std::vector<HTTPRequestHandler> http_request_hook;
    std::vector<TimerHandler> timer_hook;
    uint64_t time_stamp;

    template<typename T>
    static void append_to_hook(std::vector<T> &hook, T func) {
        hook.push_back(func);
    }

    template<typename T>
    static void insert_to_front_of_hook(std::vector<T> &hook, T func) {
        hook.insert(hook.begin(), func);
    }

    template<typename T>
    static void remove_from_hook(std::vector<T> &hook, T func) {
        for (auto i = hook.begin(); i != hook.end(); ++i) {
            if (*i == func) {
                hook.erase(i);
                return;
            }
        }
    }

private:
    std::string server_addr;
    uint16_t port_no;
    int server_sock;
    int epoll_fd;
    int timer_fd;
    int udp_fd;
    uint64_t time_out;
    std::unordered_map<int, Client *> fd_to_client;

    // name -> handle
    std::map<std::string, void *> loaded_modules;

    void do_socket();

    void do_bind();

    void do_listen();

    void do_epoll();

    void do_timer();

    void do_udp();

    void event_loop(int max_events = 128);

    void clean_old_connections();

    static void set_nonblocking(int fd);

    static void signal_handler(int dunno);

    void command_handler();

    friend class Client;
};


#endif //WEBE_SERVER_H
