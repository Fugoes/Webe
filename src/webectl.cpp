extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <dirent.h>
}

#include <cstdio>
#include <cstring>
#include <vector>
#include <iostream>
#include <set>
#include "available_modules.h"
#include "utils.h"

void do_help() {
    printf("Usage: webectl load-module <module>\n" \
           "       webectl unload-module <module>\n" \
           "       webectl list-server\n" \
    );
}

std::vector<int> get_servers() {
    auto result = std::vector<int>();
    DIR *dir;
    std::string s;
    struct dirent *ent;
    if ((dir = opendir("/tmp")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            s = ent->d_name;
            if (s.substr(0, 6) == "webed_" && s.substr(s.size() - 5) == ".sock") {
                result.push_back(atoi(s.substr(6, s.size() - 11).c_str()));
            }
        }
        closedir(dir);
        return result;
    }
    IF_FALSE_EXIT(false);
}

void send_to_pid(int pid, std::string msg) {
    int sock;
    struct sockaddr_un name;
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    IF_NEGATIVE_EXIT(sock);
    name.sun_family = AF_UNIX;
    strcpy(name.sun_path, ("/tmp/webed_" + std::to_string(pid) + ".sock").c_str());
    IF_NEGATIVE_EXIT(sendto(sock, msg.c_str(), msg.size(), 0, (struct sockaddr *)&name, sizeof(struct sockaddr_un)))
    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        do_help();
        return 0;
    }
    if (argc == 2) {
        auto cmd = std::string(argv[1]);
        if (cmd == "list-server") {
            auto pids = get_servers();
            if (pids.size() == 0) {
                printf("No server is running.\n");
            } else {
                printf("        pid\n");
                printf(" ----------\n");
                for (auto i : pids) {
                    printf(" %10d\n", i);
                }
            }
            return 0;
        } else {
            do_help();
            return 0;
        }
    }
    if (argc == 3) {
        auto cmd = std::string(argv[1]);
        auto arg = std::string(argv[2]);
        if (cmd == "load-module") {
            if (available_modules.find(arg) != available_modules.end()) {
                for (auto pid : get_servers()) {
                    send_to_pid(pid, "load-module " + arg);
                }
            }
        }
        if (cmd == "unload-module") {
            if (available_modules.find(arg) != available_modules.end()) {
                for (auto pid : get_servers()) {
                    send_to_pid(pid, "unload-module " + arg);
                }
            }
        }
    }
    return 0;
}
