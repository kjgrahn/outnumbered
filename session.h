/* -*- c++ -*-
 * $Id: session.h,v 1.6 2011-03-06 19:06:06 grahn Exp $
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

class sockaddr_storage;
class Posting;
class Command;

struct Command {
    bool process();
    bool resume();
};
class Posting {};

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

    bool readable();
    bool writable();

    bool dead() const;

    Command* initial();
    Command* command(const char* a, const char* b);
    Command* command(const std::string& s);

    const int fd_;
    sockutil::TextReader reader_;
    const std::string peer_;
    bool dead_;

    Command* pending_;
    std::queue<std::string> backlog_;

    Posting* posting_;
};

#endif
