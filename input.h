/* -*- c++ -*-
 *
 * Copyright (c) 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_INPUT_H_
#define GB_INPUT_H_

#include <string>

/**
 * A wrapper for open(file, O_RDONLY), Like opening a Unix fd for
 * reading, but keeps track of "have I seen EOF" yet? and stuff.
 *
 * alive() means there's a chance we can squeeze more data from this
 * input.
 */
class InputFd {
public:
    explicit InputFd(const std::string& path);
    ~InputFd();
    size_t read(void *buf, size_t count);
    bool alive() const { return fd>=0 && !eof && !err; }

private:
    InputFd(const InputFd&);
    InputFd& operator= (const InputFd&);
    const int fd;
    bool eof;
    int err;
};


#endif
