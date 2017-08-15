#ifndef WEBE_MODULE_UTILS_H_H
#define WEBE_MODULE_UTILS_H_H

#include <string>

template <const char *path, int length>
static bool match_path(const std::string &uri) {
    int i;
    if (uri.size() < length) return false;
    for (i = 0; i < length; i++) {
        if (uri[i] != path[i]) {
            return false;
        }
    }
    if (uri.size() == length) return true;
    return uri[length] == '/';
}

#endif //WEBE_MODULE_UTILS_H_H
