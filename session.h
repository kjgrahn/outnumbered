/* -*- c++ -*-
 * $Id: session.h,v 1.11 2011-07-03 16:13:11 grahn Exp $
 *
 * Copyright (c) 2010, 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_SESSION_H_
#define GB_SESSION_H_

#include <string>
#include <queue>
#include <cstdlib>
#include <ctime>

#include <sockutil/textread.h>
#include "responsebuf.h"


class sockaddr_storage;
class Posting;

class Posting {};

/*
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


/**
 * An NNTP session. Also the TCP client; event handling is done
 * higher up, but this one owns the fd, and does the reading/writing.
 *
 * Its externally visible state is:
 * - READING: I want to read; notify me when the fd is readable.
 * - blocked WRITING: I need to write, but the write would block.
 *   Notify me when the fd is writable again.
 * - DEAD: the NNTP session has died (either controlledly or by
 *   I/O error); the fd and this Session are both useless.
 */
class Session {
public:
    Session(int fd, const sockaddr_storage& sa);
    ~Session();

    enum State { READING, WRITING, DEAD };
    State event();

    int fd() const { return fd_; }
    time_t atime() const;

private:
    Session();
    Session(const Session&);
    Session& operator= (const Session& other);

    void readable();
    void writable();
    void flush();

    void initial();
    void command(const char* a, const char* b);

    const int fd_;
    sockutil::TextReader reader_;
    ResponseBuf writer_;
    const std::string peer_;
    bool dead_;

    std::queue<std::string> backlog_;

    Posting* posting_;
};

#endif
