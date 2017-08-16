#include "thread_info.h"
#include <experimental/filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

namespace fs = std::experimental::filesystem;

std::vector<std::string> list_dirs(std::string path) {
    std::vector<std::string> result;
    for (auto & p : fs::directory_iterator(path)) {
        if (fs::is_directory(p)) {
            result.push_back(std::string(p.path()));
        }
    }
    return result;
}

std::vector<std::string> list_dirs(const char * path) {
    return list_dirs(std::string(path));
}

std::vector<std::tuple<int, std::string, std::string>> get_threads_info() {
    // pid, name, status
    std::vector<std::tuple<int, std::string, std::string>> result;
    std::regex rx("^/proc/(\\d+)$");
    std::regex rx_state("^.*\\((\\w+)\\)$");
    std::smatch matches;
    auto dirs = list_dirs("/proc");
    int pid;
    std::string name;
    std::string state;
    std::ifstream file;
    for (auto && dir : dirs) {
        if (std::regex_match(dir, matches, rx)) {
            file.open(dir + "/status");
            file >> name >> name;
            getline(file, state);
            getline(file, state);
            getline(file, state);
            std::istringstream(matches.str(1)) >> pid;
            std::regex_match(state, matches, rx_state);
            state = matches.str(1);
            result.push_back(std::make_tuple(pid, name, state));
            file.close();
        }
    }
    return result;
}

std::tuple<int, int, int> get_running_sleeping_total_threads() {
    int running = 0;
    int sleeping = 0;
    int total = 0;
    for (auto & thread : get_threads_info()) {
        if (std::get<2>(thread) == "sleeping") {
            sleeping++;
        } else if (std::get<2>(thread) == "running") {
            running++;
        }
        total++;
    }
    return std::make_tuple(running, sleeping, total);
}

void test() {
    auto info = get_running_sleeping_total_threads();
    std::cout << std::get<0>(info) << "," << std::get<1>(info)
        << "," << std::get<2>(info) << std::endl;
}
