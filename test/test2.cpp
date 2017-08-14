#include "../src/read_buffer.h"
#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

int main() {
    auto file = open("/tmp/fifo", O_RDONLY | O_NONBLOCK | O_CREAT);
    ReadBuffer b(file);
    const char *c;
    ssize_t size;
    int i = 0;
    while (1) {
        usleep(100);
        if (b.do_read() == 0) {
            while (1) {
                std::tie(c, size) = b.get_chars(10);
                if (c == nullptr) {
                    break;
                }
                i++;
                printf("%3d %3d ", i, size);
                std::cout << std::string(c, size) << std::endl;
            }
        }
    }
    return 0;
}