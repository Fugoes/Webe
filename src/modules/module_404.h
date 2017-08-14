#ifndef WEBE_MODULE_404_H
#define WEBE_MODULE_404_H

#include "../client.h"
#include "../server.h"

extern "C" {
HTTPResponse *http_request_handler(Client *client);
};

#endif //WEBE_MODULE_404_H
