#include "module_base.h"
#include "module_utils.h"
#include <cinttypes>
#include <fstream>
#include <sys/statvfs.h>
#include <boost/filesystem.hpp>
#include "thread_info.h"

static uint64_t mem_total, mem_free, mem_available;
static boost::filesystem::space_info disk_info;
static std::vector<std::tuple<int, std::string, std::string>> processes_info;
static std::string processes_data;
static std::string version;
static HTTPResponse *http_request_handler(Client *client);

static constexpr char p[] = "/info";

static void refresh_disk_info() {
    disk_info = boost::filesystem::space("/");
}

static void refresh_mem_info() {
    std::ifstream file;
    std::stringstream ss;
    std::string s;

    file.open("/proc/meminfo", std::fstream::in);
    file >> s >> mem_total >> s >> s >> mem_free >> s >> s >> mem_available;
}

static void refresh_thread_info() {
    processes_info = get_threads_info();
    processes_data.assign("");
    for (auto & i : processes_info) {
        processes_data += "<tr><th align=\"left\">" + std::to_string(std::get<0>(i)) + "</th><td align=\"right\">" + std::get<1>(i) + " </td></tr>";
    }
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
    refresh_disk_info();
    refresh_thread_info();
}

int module_load(Server *server) {
    printf("Loading module info...\n");
    get_version_info();
    refresh_mem_info();
    refresh_disk_info();
    refresh_thread_info();
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
                "<h1>System Information</h1>" \
                  "<div class=\"entry\">" \
                    "<input type=\"button\" value=\"Kernel Version\" class=\"button\" onclick=\"ToggleBetweenDisplayAndHide('KernelVersion')\"/>" \
                    "<div id=\"KernelVersion\" class=\"hide\">" \
                    "<div class=\"text\">" + version + \
                    "</div>" \
                    "</div>" \
                  "</div>" \
                  "<div class=\"entry\">" \
                    "<input type=\"button\" value=\"Memory Status\" class=\"button\" onclick=\"ToggleBetweenDisplayAndHide('MemoryUsage')\"/>" \
                    "<div id=\"MemoryUsage\" class=\"hide\">" \
                    "<table id=\"MemoryUsageTable\" class=\"table\" border=\"0\" style=\"width: 40%\">" \
                    "<tr><th align=\"left\">Total</th><td align=\"right\">" + std::to_string(mem_total / 1024) + " MiB</td></tr>" \
                    "<tr><th align=\"left\">Free</th><td align=\"right\">" + std::to_string(mem_free / 1024) + " MiB</td></tr>" \
                    "<tr><th align=\"left\">Available</th><td align=\"right\">" + std::to_string(mem_available / 1024) + " MiB</td></tr>" \
                    "</table>" \
                    "</div>" \
                  "</div>" \
                  "<div class=\"entry\">" \
                    "<input type=\"button\" value=\"Disk Status\" class=\"button\" onclick=\"ToggleBetweenDisplayAndHide('DiskStatus')\"/>" \
                    "<div id=\"DiskStatus\" class=\"hide\">" \
                    "<table class=\"table\" border=\"0\" style=\"width: 40%\">" \
                    "<tr><th align=\"left\">Capacity</th><td align=\"right\">" + std::to_string(disk_info.capacity / 1024 / 1024) + " MiB</td></tr>" \
                    "<tr><th align=\"left\">Free</th><td align=\"right\">" + std::to_string(disk_info.free / 1024 / 1024) + " MiB</td></tr>" \
                    "<tr><th align=\"left\">Available</th><td align=\"right\">" + std::to_string(disk_info.available / 1024 / 1024) + " MiB</td></tr>" \
                    "</table>" \
                    "</div>" \
                  "</div>" \
                  "<div class=\"entry\">" \
                    "<input type=\"button\" value=\"Processes Status\" class=\"button\" onclick=\"ToggleBetweenDisplayAndHide('ProcessesStatus')\"/>" \
                    "<div id=\"ProcessesStatus\" class=\"hide\">" \
                    "<table class=\"table\" border=\"0\" style=\"width: 40%\">" \
                    "<tr><th align=\"left\">Total</th><td align=\"right\">" + std::to_string(processes_info.size()) + "</td></tr>" \
                    "<tr><th align=\"left\">PID</th><td align=\"right\">NAME</td></tr>" \
                    + processes_data + "</table>" \
                    "</div>" \
                  "</div>" \
                "<div class=\"text\" align=\"right\">" \
                "&hearts; Fugoes" \
                "</div>"
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
