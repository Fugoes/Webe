#include "module_base.h"
#include "module_utils.h"

static constexpr char p[] = "/static";

static HTTPResponse *http_request_handler(Client *client) {

}

int module_load(Server *server) {
    printf("Loading module FileServer...\n");
    Server::insert_to_front_of_hook(server->http_request_hook, &http_request_handler);
    printf("Loaded module FileServer\n");
}

int module_unload(Server *server) {
    printf("Unloading module FileServer\n");
    Server::remove_from_hook(server->http_request_hook, &http_request_handler);
    printf("Unloaded module FileServer\n");
}
