/* -*- c++ -*-
 *
 * Copyright (c) 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_PIPE_H_
#define GB_PIPE_H_

#include <string>

/**
 * A Unix O_NONBLOCK pipe, for test purposes.
 * Replacement for the write end of a TCP socket.
 */
class Pipe {
public:
    Pipe();
    ~Pipe();

    int fd() const { return wfd; }
    void assert_read(const std::string& s);
    void assert_drain(size_t n);
    void assert_empty();

private:
    int rfd;
    int wfd;
};

#endif
