#ifndef WEBE_MODULES_H_H
#define WEBE_MODULES_H_H

#include "client.h"
#include "server.h"
#include "http_request.h"
#include "http_response.h"

typedef HTTPResponse* (*HTTPRequestHandler)(Client *client);
typedef void (*TimerHandler)(Server *server);
typedef int (*ModuleLoad)(Server *server);
typedef int (*ModuleUnload)(Server *server);

const std::map<std::string, std::tuple<bool, bool>> available_modules = {
        {"404", std::make_tuple(true, false)},
        {"info", std::make_tuple(true, true)},
};

#endif //WEBE_MODULES_H_H
