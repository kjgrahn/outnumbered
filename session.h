/* -*- c++ -*-
 * $Id: session.h,v 1.3 2010-07-25 20:20:35 grahn Exp $
 *
 * Copyright (c) 2010 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_CLIENT_H_
#define GB_CLIENT_H_

#include <cstdlib>
#include <string>

#include <sockutil/textread.h>

class sockaddr_storage;


/**
 * The NNTP client. Also the TCP client; event handling is done
 * higher up, but this one owns the fd, and does the reading/writing.
 */
class Client {
public:
    Client(int fd, const sockaddr_storage& sa);
    ~Client();

    void feed();
    bool eof() const;

    int fd() const { return fd_; }

private:
    Client();
    Client(const Client&);
    Client& operator= (const Client& other);

    int fd_;
    sockutil::TextReader* reader_;
    std::string peer_;
};

#endif
