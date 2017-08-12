#ifndef WEBE_CLIENT_H
#define WEBE_CLIENT_H

#include <string>

class Client;

#include "server.h"

#define CLIENT_BUFFER_SIZE 65536


class Client {
public:
    static int client_new;
    static int client_delete;

    Client(int fd, std::string addr, uint16_t port_no, Server *server);

    ~Client();

    static void handle_in(Client *self);

    static void handle_rdhup(Client *self);

private:
    Server *server;
    std::string addr;
    uint16_t port_no;
    uint64_t time_stamp;
    int fd;
    char buffer[CLIENT_BUFFER_SIZE];

    friend class Server;
};

#endif //WEBE_CLIENT_H
