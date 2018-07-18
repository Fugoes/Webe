#include "module_base.h"
#include "module_utils.h"
#include <string>
#include <list>
#include <map>
#include <iostream>
#include <cstring>
#include <stdio.h>

#include "dirent.h"

static constexpr char p[] = "/static";

static std::map<std::string, std::tuple<const char *, ssize_t, std::string>> filename_to_buffer;

static HTTPResponse *http_request_handler(Client *client) {
    auto m = filename_to_buffer.find(client->request.uri.substr(1));
    if (client->request.uri[0] == '/'
        && (m != filename_to_buffer.end())) {
        auto response = new HTTPResponse();
        response->version = "HTTP/1.1";
        response->status = "200 OK";
        response->data.assign(std::get<0>(m->second), (size_t)std::get<1>(m->second));
        response->header.push_back(std::make_tuple("Server", "Webe/0.1"));
        response->header.push_back(std::make_tuple("Date", HTTPResponse::date()));
        response->header.push_back(std::make_tuple("Content-Type", std::get<2>(m->second)));
        response->header.push_back(std::make_tuple("Content-Length", std::to_string(response->data.size())));
        response->header.push_back(std::make_tuple("Connection", "keep-alive"));
        return response;
    } else return nullptr;
}

static std::list<std::string> recursive_list_files(std::string path) {
    std::list<std::string> result;
    std::list<std::string> todo;
    std::string s;
    todo.push_back(path);

    DIR *dir;
    struct dirent *ent;

    while (todo.size() > 0) {
        auto item = todo.back();
        todo.pop_back();
        if ((dir = opendir(item.c_str())) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                s = ent->d_name;
                if (s == "." || s == ".." || s[0] == '.') continue;
                switch (ent->d_type) {
                    case DT_REG:
                        // is regular file
                        result.push_back(item + "/" + s);
                        break;
                    case DT_DIR:
                        // is directory
                        todo.push_back(item + "/" + s);
                        break;
                    default: IF_FALSE_EXIT(false);
                }
            }
            closedir(dir);
        }
    }
    return result;
}

static std::tuple<const char *, ssize_t> load_file(const std::string &file) {
    auto f = fopen(file.c_str(), "r");
    fseek(f, 0, SEEK_END);
    auto sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    auto buf = new char[sz];
    fread(buf, sz, sz, f);
    fclose(f);
    return std::make_tuple(buf, sz);
};

static void load_file_into_mem() {
    const char *buf;
    ssize_t size;
    auto file_list = recursive_list_files("static");
    for (auto && file : file_list) {
        if (file.size() > 6 && file.substr(file.size() - 5) == ".html") {
            std::tie(buf, size) = load_file(file);
            std::cout << file << std::endl;
            filename_to_buffer[file] = std::make_tuple(buf, size, "text/html");
        } else if (file.size() > 5 && file.substr(file.size() - 4) == ".css") {
            std::tie(buf, size) = load_file(file);
            std::cout << file << std::endl;
            filename_to_buffer[file] = std::make_tuple(buf, size, "text/css");
        } else if (file.size() > 4 && file.substr(file.size() - 3) == ".js") {
            std::tie(buf, size) = load_file(file);
            std::cout << file << std::endl;
            filename_to_buffer[file] = std::make_tuple(buf, size, "application/javascript");
        }
    }
}

static void unload_file_from_mem() {
    for (auto &i : filename_to_buffer) {
        delete[] std::get<0>(i.second);
    }
}

int module_load(Server *server) {
    printf("Loading module FileServer...\n");
    load_file_into_mem();
    Server::insert_to_front_of_hook(server->http_request_hook, &http_request_handler);
    printf("Loaded module FileServer\n");
    return 0;
}

int module_unload(Server *server) {
    printf("Unloading module FileServer...\n");
    unload_file_from_mem();
    Server::remove_from_hook(server->http_request_hook, &http_request_handler);
    printf("Unloaded module FileServer\n");
    return 0;
}
