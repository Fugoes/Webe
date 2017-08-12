#ifndef WEBE_CLIENT_H
#define WEBE_CLIENT_H

#include <string>

#define CLIENT_BUFFER_SIZE 65536


class Client {
public:
    Client(int fd, std::string addr, uint16_t port_no);
    ~Client();
    static int client_new;
    static int client_delete;

    static void handle_in(Client *self);
    static void handle_rdhup(Client *self);

private:
    std::string addr;
    uint16_t port_no;

    int fd;
    char buffer[CLIENT_BUFFER_SIZE];
};

#endif //WEBE_CLIENT_H
