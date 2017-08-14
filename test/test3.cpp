#include "../src/read_buffer.h"
#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

int main() {
    auto pull = open("/tmp/fifo", O_RDONLY | O_NONBLOCK | O_CREAT, 0644);
    ReadBuffer b(pull);

    const char *c;
    ssize_t size;

    while (1) {
        usleep(100);
        if (b.do_read() == 0) {
            std::tie(c, size) = b.get_line();
            if (c != nullptr) {
                printf("%3d ", size);
                std::cout << std::string(c, size) << std::endl;
            }
        }
    }

    return 0;
}