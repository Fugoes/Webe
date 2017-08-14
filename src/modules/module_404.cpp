#include "module_404.h"
#include <iostream>

HTTPResponse *http_request_handler(Client *client) {
    auto result = new HTTPResponse;
    if (client->request.method == "GET" || client->request.method == "HEAD") {
        result->header.set_status("404 Not Found");
        result->header.append("Server", "Webe/0.1");
        result->header.append("Date", HTTPResponseHeader::date());
        result->header.append("Content-Type", "text/html");
        result->header.append("Content-Length", "165");
        result->header.append("Connection", "keep-alive");
        result->data =
                "<html>\r\n"
                "<head><title>404 Not Found</title></head>\r\n"
                "<body bgcolor=\"white\">\r\n"
                "<center><h1>404 Not Found</h1></center>\r\n"
                "<hr><center>Webe/0.1</center>\r\n"
                "</body>\r\n"
                "</html>\r\n";
        result->parse();
        return result;
    } else {
        return nullptr;
    }
}
