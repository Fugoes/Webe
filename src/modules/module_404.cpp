#include "module_404.h"
#include <iostream>

HTTPResponse *http_request_handler(Client *client) {
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
    response->header.push_back(std::make_tuple("Data", HTTPResponse::date()));
    response->header.push_back(std::make_tuple("Content-Type", "text/html"));
    response->header.push_back(std::make_tuple("Content-Length", std::to_string(response->data.size())));
    response->header.push_back(std::make_tuple("Connection", "keep-alive"));
    return response;
}

int module_load(Server *server) {
    server->http_request_hook.push_back(&http_request_handler);
}

int module_unload(Server *server) {
    for (auto i = server->http_request_hook.begin(); i != server->http_request_hook.end(); i++) {
        if (*i == &http_request_handler) {
            server->http_request_hook.erase(i);
            return 0;
        }
    }
}
