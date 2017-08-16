#include "module_base.h"
#include "module_utils.h"
#include <cinttypes>
#include <fstream>

static uint64_t mem_total, mem_free, mem_available;
static std::string version;
static HTTPResponse *http_request_handler(Client *client);

static constexpr char p[] = "/info";

static void refresh_mem_info() {
    std::ifstream file;
    std::stringstream ss;
    std::string s;

    file.open("/proc/meminfo", std::fstream::in);
    file >> s >> mem_total >> s >> s >> mem_free >> s >> s >> mem_available;
}

static void get_version_info() {
    std::ifstream file;
    file.open("/proc/version", std::fstream::in);
    std::getline(file, version);
}

static void refresh_cpu_info() {
    std::ifstream file;
    std::stringstream ss;
    std::string s;
}

static void timer_handler(Server *server) {
    refresh_mem_info();
    get_version_info();
}

int module_load(Server *server) {
    printf("Loading module info...\n");
    refresh_mem_info();
    Server::append_to_hook(server->timer_hook, &timer_handler);
    Server::insert_to_front_of_hook(server->http_request_hook, &http_request_handler);
    printf("Loaded module info\n");
    return 0;
}

int module_unload(Server *server) {
    printf("Unloading module info...\n");
    Server::remove_from_hook(server->timer_hook, &timer_handler);
    Server::remove_from_hook(server->http_request_hook, &http_request_handler);
    printf("Unloaded module info\n");
    return 0;
}

static HTTPResponse *http_request_handler(Client *client) {
    if (match_path<p, 5>(client->request.uri)) {
        auto response = new HTTPResponse();
        response->version = "HTTP/1.1";
        response->status = "200 OK";
        response->data = \
                "<!DOCTYPE html>" \
                "<html>" \
                "<head>" \
                "<link rel=\"stylesheet\" type=\"text/css\" href=\"/static/nice.css\" media=\"screen\"/>" \
                "<script src=\"/static/nice.js\"></script>" \
                "</head>" \
                "<title>System Information</title>" \
                "<body>" \
                "<h1>Hello World!</h1>" \
                "<div class=\"entry\">" \
                "<input type=\"button\" value=\"Memory Usage\" class=\"button\" onclick=\"divDisplayAndHide('MemoryUsage')\"/>" \
                "<div id=\"MemoryUsage\" class=\"display\">" \
                "<table id=\"MemoryUsageTable\" class=\"table\" border=\"0\" style=\"width: 30%\">" \
                "<tr><th align=\"left\">Free</th><td align=\"right\">" + std::to_string(mem_free / 1024) + " MiB</td></tr>" \
                "<tr><th align=\"left\">Available</th><td align=\"right\">" + std::to_string(mem_available / 1024) + " MiB</td></tr>" \
                "</table>" \
                "</div>" \
                "</div>" \
                "</body>" \
                "</html>";

        response->header.push_back(std::make_tuple("Server", "Webe/0.1"));
        response->header.push_back(std::make_tuple("Data", HTTPResponse::date()));
        response->header.push_back(std::make_tuple("Content-Type", "text/html"));
        response->header.push_back(std::make_tuple("Content-Length", std::to_string(response->data.size())));
        response->header.push_back(std::make_tuple("Connection", "keep-alive"));
        return response;
    } else {
        return nullptr;
    }
}
