#ifndef WEBE_UTILS_H
#define WEBE_UTILS_H

#include <cassert>
#include <cstdlib>
#include <cstdio>

#define IF_NEGATIVE_EXIT(EXP)                           \
    if (EXP < 0) {                                      \
        fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, "--> %s\n", #EXP);              \
        exit(1);                                        \
    }

#endif //WEBE_UTILS_H
