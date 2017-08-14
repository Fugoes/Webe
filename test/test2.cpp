#include "../src/read_buffer.h"
#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

int main() {
    auto file = open("/tmp/fifo", O_RDONLY | O_NONBLOCK);
    ReadBuffer b(file);
    const char *c;
    ssize_t size;
    int i = 0;
    while (1) {
        std::tie(c, size) = b.get_line();
        if (c == nullptr) {
            usleep(10);
            continue;
        }
        i++;
        printf("%3d %3d ", i, size);
        std::cout << std::string(c, size) << std::endl;
    }
    return 0;
}