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

#define IF_FALSE_EXIT(EXP)                              \
    if (!(EXP)) {                                       \
        fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, "--> %s\n", #EXP);              \
        perror("info");                                 \
        exit(1);                                        \
    }

#endif //WEBE_UTILS_H
