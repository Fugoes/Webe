#ifndef WEBE_UTILS_H
#define WEBE_UTILS_H

#include <cassert>
#include <cstdlib>
#include <cstdio>

#define IF_NEGATIVE_EXIT(EXP)                   \
    if (EXP < 0) {                              \
        fprintf(stderr, "%s:%d",                \
                __FILE__, __LINE__);            \
        perror(#EXP);                           \
        exit(1);                                \
    }

#endif //WEBE_UTILS_H
