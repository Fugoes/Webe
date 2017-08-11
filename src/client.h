#ifndef WEBE_CLIENT_H
#define WEBE_CLIENT_H

#include <string>


class Client {
public:
private:
    std::string addr;
    uint16_t port_no;

    int fd;
};


#endif //WEBE_CLIENT_H
