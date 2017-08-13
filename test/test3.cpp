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
        auto flag = b.do_read();
        if (flag == EBUSY) {
            while (1) {
                std::tie(c, size) = b.get_chars(10);
                if (c == nullptr) break;
                printf("%3d %3d ", i++, size);
                std::cout << std::string(c, size) << std::endl;
            }
            b.do_move();
        }
        if (flag == EAGAIN || flag == 0) {
            while (1) {
                std::tie(c, size) = b.get_chars(10);
                if (c == nullptr) break;
                printf("%3d %3d ", i++, size);
                std::cout << std::string(c, size) << std::endl;
            }
            b.do_move();
            usleep(100);
        }
    }
    return 0;
}