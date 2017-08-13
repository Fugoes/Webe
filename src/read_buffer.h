#ifndef WEBE_BUFFER_H
#define WEBE_BUFFER_H


#include <stdio.h>
#include <tuple>

const int buffer_size = 30;

class ReadBuffer {
public:
    ReadBuffer(int fd);

    /**
     * get a line by \r\n
     * @return
     * 1. char * point to first element of the line
     * 2. length of the line
     * If failed, it will return (nullptr, 0)
     */
    std::tuple<const char *, ssize_t> get_line();

    /**
     * Try to get n chars
     * @param n
     * @return
     * 1. char * point to first element of the chars
     * 2. length of the chars
     * If cannot get, return (nullptr, 0)
     */
    std::tuple<const char *, ssize_t> get_chars(ssize_t n);

    /**
     * read
     * @return
     * EBUSY if buffer is full
     * EAGAIN if cannot read from fd
     */
    int do_read();

    void do_move();

    /**
     * give up all chars in buffer
     */
    void do_flush();

    char buffer[buffer_size];
    ssize_t left, right;

    int fd;
};


#endif //WEBE_BUFFER_H
