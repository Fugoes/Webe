#ifndef WEBE_UTILS_H
#define WEBE_UTILS_H

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <sstream>

#define IF_NEGATIVE_EXIT(EXP)                           \
    if (EXP < 0) {                                      \
        fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, "--> %s\n", #EXP);              \
        perror("info");                                 \
        exit(1);                                        \
    }

int string_to_int(const std::string str) {
    int result;
    std::stringstream ss;
    ss << str;
    ss >> result;
    return result;
}

#endif //WEBE_UTILS_H
