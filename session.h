/* -*- c++ -*-
 * $Id: session.h,v 1.5 2011-03-04 22:55:12 grahn Exp $
 *
 * Copyright (c) 2010, 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_SESSION_H_
#define GB_SESSION_H_

#include <cstdlib>
#include <string>

#include <sockutil/textread.h>

class sockaddr_storage;


/**
 * An NNTP session. Also the TCP client; event handling is done
 * higher up, but this one owns the fd, and does the reading/writing.
 */
class Session {
public:
    Session(int fd, const sockaddr_storage& sa);
    ~Session();

    void feed();
    bool eof() const;

    int fd() const { return fd_; }

private:
    Session();
    Session(const Session&);
    Session& operator= (const Session& other);

    void read(const char* a, const char* b);

    const int fd_;
    sockutil::TextReader reader_;
    const std::string peer_;
    std::string backlog_;
};

#endif
