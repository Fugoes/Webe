#ifndef WEBE_CLIENT_H
#define WEBE_CLIENT_H

#include <string>

class Client;

#include "server.h"
#include "http_request.h"

class Client {
public:
    static int client_new;
    static int client_delete;
    HTTPRequest request;

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

    friend class Server;
};

#endif //WEBE_CLIENT_H
