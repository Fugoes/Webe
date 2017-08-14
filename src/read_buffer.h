#ifndef WEBE_BUFFER_H
#define WEBE_BUFFER_H


#include <stdio.h>
#include <tuple>

class ReadBuffer {
public:
    ReadBuffer(int fd);
    ~ReadBuffer();

    void do_move();

    /**
     * read till EAGAIN
     */
    int do_read();

    /**
     * get a line by \r\n
     * @return
     * 1. char * point to first element of the line
     * 2. length of the line
     * If failed because the buffer EAGAIN, return (nullptr, 0)
     * If failed because the line is too long, throw "ErrorGetLine"
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
     * give up all chars in buffer
     */
    void do_flush();

private:
    char *buffer;
    ssize_t buffer_size;
    ssize_t left, right;
    int fd;
};


#endif //WEBE_BUFFER_H
