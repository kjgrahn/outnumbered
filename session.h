/* -*- c++ -*-
 * $Id: session.h,v 1.11 2011-07-03 16:13:11 grahn Exp $
 *
 * Copyright (c) 2010, 2011, 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_SESSION_H_
#define GB_SESSION_H_

#include <iosfwd>
#include <netinet/in.h>
#include <sys/epoll.h>

#include "times.h"
#include "textread.h"
#include "requestqueue.h"
#include "response.h"


/**
 * A HTTP session, or more generally a pipelined request-response TCP
 * session using nonblocking I/O.
 *
 * The logical view is two possibly infinite sequences of requests and
 * responses. A read may give use 0--N complete requests. A write may
 * complete a response.
 *
 *     +--------           +--------
 *     | req               | resp
 *     +--------           |
 *     | req               |
 *     |                   |
 *     +--------           |
 *     | req               +--------
 *     +--------           | resp
 *     | req               +--------
 *     ········ <== read   | resp
 *     |                   |
 *     |         write ==> ········
 *     +--------           |
 *     | req               |
 *     |                   |
 *     +--------           |
 *     | req               |
 *     +--------           |
 *     | req               |
 *     |                   +--------
 *     |                   | resp
 *     |                   |
 *     | ...               |
 *                         |
 *                         | ...
 *
 * At any moment there's a queue of 0--N complete Requests, the oldest of
 * which is associated with a Response, the goal of which is to empty
 * itself into the fd.
 *
 * The interface is simply three functions:
 * - the fd is readable
 * - the fd is writable
 * - have you had no success lately?
 *
 */
class Session {
public:
    explicit Session(const sockaddr_storage& peer);
    ~Session();

    enum State {
	DIE = 0,
	READING = EPOLLIN,
	WRITING = EPOLLOUT
    };

    State read(int fd, const timespec& t);
    State write(int fd, const timespec& t);
    State reconsider(const timespec& t);

    std::ostream& put(std::ostream& os) const;

private:
    Session();
    Session(const Session&);
    Session& operator= (const Session& other);

    sockaddr_storage peer;
    sockutil::TextReader reader;
    RequestQueue queue;
    Response* response;
};


inline
std::ostream& operator<< (std::ostream& os, const Session& val)
{
    return val.put(os);
}

#endif
