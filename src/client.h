#ifndef WEBE_CLIENT_H
#define WEBE_CLIENT_H

#include <string>

class Client;

#include "server.h"

#define CLIENT_BUFFER_SIZE 65536


class Client {
public:
    Client(int fd, std::string addr, uint16_t port_no);
    ~Client();
    static int client_new;
    static int client_delete;

    static void handle_in(Client *self, Server *server);
    static void handle_rdhup(Client *self, Server *server);

private:
    std::string addr;
    uint16_t port_no;

    uint64_t time_stamp;

    int fd;
    char buffer[CLIENT_BUFFER_SIZE];

    friend class Server;
};

#endif //WEBE_CLIENT_H
