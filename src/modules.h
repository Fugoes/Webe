#ifndef WEBE_MODULES_H_H
#define WEBE_MODULES_H_H

#include "client.h"
#include "server.h"
#include "http_request.h"
#include "http_response.h"
#include <set>

typedef int (*ModuleLoad)(Server *server);
typedef int (*ModuleUnload)(Server *server);

typedef HTTPResponse* (*HTTPRequestHandler)(Client *client);
typedef void (*TimerHandler)(Server *server);

const std::set<std::string> available_modules = {
        "404",
        "info",
        "file_server",
};

#endif //WEBE_MODULES_H_H
