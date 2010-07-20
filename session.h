/* -*- c++ -*-
 * $Id: session.h,v 1.2 2010-07-20 14:34:39 grahn Exp $
 *
 * Copyright (c) 2010 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_CLIENT_H_
#define GB_CLIENT_H_

#include <cstdlib>

class sockaddr_storage;

/**
 * Per-client NNTP state.
 */
class Client {
public:
    Client();
    Client(int fd, const sockaddr_storage& sa);

    bool operator== (const Client& other) const;

private:
};

#endif
