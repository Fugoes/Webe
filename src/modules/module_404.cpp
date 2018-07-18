#include "module_base.h"
#include <iostream>
#include <cinttypes>

static HTTPResponse *http_request_handler(Client *client) {
    auto response = new HTTPResponse();
    response->version = "HTTP/1.1";
    response->status = "404 Not Found";
    response->data = "<html>"
            "<head><title>404 Not Found</title></head>"
            "<body bgcolor=\"white\">"
            "<center><h1>404 Not Found</h1></center>"
            "<hr><center>Webe/0.1</center>"
            "</body>"
            "</html>";

    response->header.push_back(std::make_tuple("Server", "Webe/0.1"));
    response->header.push_back(std::make_tuple("Date", HTTPResponse::date()));
    response->header.push_back(std::make_tuple("Content-Type", "text/html"));
    response->header.push_back(std::make_tuple("Content-Length", std::to_string(response->data.size())));
    response->header.push_back(std::make_tuple("Connection", "keep-alive"));
    return response;
}

int module_load(Server *server) {
    printf("Loading Module 404...\n");
    Server::append_to_hook<HTTPRequestHandler>(server->http_request_hook, &http_request_handler);
    printf("Loaded Module 404\n");
    return 0;
}

int module_unload(Server *server) {
    printf("Unloading Module 404...\n");
    Server::remove_from_hook<HTTPRequestHandler>(server->http_request_hook, &http_request_handler);
    printf("Unloaded Module 404\n");
    return 0;
}
