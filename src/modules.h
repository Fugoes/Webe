#ifndef WEBE_MODULES_H_H
#define WEBE_MODULES_H_H

#include "client.h"
#include "server.h"
#include "http_request.h"
#include "http_response.h"

typedef HTTPResponse* (*handle_http_request)(Client *client);

#endif //WEBE_MODULES_H_H
