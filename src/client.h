#ifndef WEBE_CLIENT_H
#define WEBE_CLIENT_H

#include <string>

#define CLIENT_BUFFER_SIZE 65536


class Client {
public:
    Client(int fd, std::string addr, uint16_t port_no);

    void handle_in();

private:
    std::string addr;
    uint16_t port_no;

    int fd;
    char buffer[CLIENT_BUFFER_SIZE];
};


#endif //WEBE_CLIENT_H
