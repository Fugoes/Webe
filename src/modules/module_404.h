#ifndef WEBE_MODULE_404_H
#define WEBE_MODULE_404_H

#include "../client.h"
#include "../server.h"

extern "C" {
int module_load(Server *server);
int module_unload(Server *server);
};

#endif //WEBE_MODULE_404_H
