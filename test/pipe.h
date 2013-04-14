/* -*- c++ -*-
 *
 * Copyright (c) 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_PIPE_H_
#define GB_PIPE_H_

#include <string>

class Blob;

/**
 * A Unix O_NONBLOCK pipe, for test purposes.
 * Replacement for the write end of a TCP socket.
 *
 * Not a perfect replacement; pipe(7) describes how it's hard
 * to accomplish a partial write on a pipe.
 */
class Pipe {
public:
    Pipe();
    ~Pipe();

    int fd() const { return wfd; }
    void assert_read(const std::string& s);
    void assert_read(const Blob& s);
    size_t drain(size_t len);
    void assert_drain(size_t len);
    void assert_empty();

private:
    int rfd;
    int wfd;
};

#endif
