/* $Id: session.cc,v 1.4 2011-03-06 19:06:06 grahn Exp $
 *
 * Copyright (c) 2010, 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "session.h"

#include "response.h"

#include <iostream>
#include <sstream>
#include <cassert>

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>


namespace {

    std::ostream& operator<< (std::ostream& os, const sockaddr_storage& sa)
    {
	char hbuf[300];
	char sbuf[30];
	int rc = getnameinfo(reinterpret_cast<const struct sockaddr*>(&sa),
			     sizeof sa,
			     hbuf, sizeof hbuf,
			     sbuf, sizeof sbuf,
			     NI_NOFQDN
			     | NI_NUMERICHOST
			     | NI_NUMERICSERV);
	if(rc) {
	    return os << "unknown(" << strerror(errno) << ')';
	}
	return os << hbuf << ':' << sbuf;
    }

    std::string getnameinfo(const sockaddr_storage& sa)
    {
	std::ostringstream oss;
	oss << sa;
	return oss.str();
    }
}


Session::Session(int fd, const sockaddr_storage& sa)
    : fd_(fd),
      reader_(sockutil::TextReader(fd, "\r\n")),
      peer_(getnameinfo(sa)),
      dead_(false),
      pending_(0),
      posting_(0)
{
    Command* const cmd = initial();
    if(cmd->process()) {
	delete cmd; 
    }
    else {
	/* XXX bug: we stay in state READING */
	pending_ = cmd;
    }
}


Session::~Session()
{
    delete pending_;
    delete posting_;
    close(fd_);
}


/**
 * We claimed to be READING, and the TCP socket is now readable.
 * The result is that:
 * - we've read all N complete lines
 * - we've processed M of them (as commands or lines of a posting)
 * and we may have moved to state WRITING, in which case:
 * - one command is left pending because we're blocked writing
 * - M lines ended up in the backlog
 * or we moved to state DEAD.
 *
 * Returns true iff we don't leave the state.
 */
bool Session::readable()
{
    assert(!dead_);
    assert(!pending_);
    assert(backlog_.empty());

    reader_.feed();

    char* a;
    char* b;
    while(reader_.read(a, b)) {
	Command* const cmd = command(a, b);
	if(cmd->process()) {
	    delete cmd; 
	}
	else {
	    pending_ = cmd;
	    break;
	}
    }

    if(pending_) {
	while(reader_.read(a, b)) {
	    backlog_.push(std::string(a, b));
	}
    }

    /* XXX maybe too strict; what if we have stuff to write? */
    dead_ |= reader_.eof();

    return !pending_ && !dead_;
}


/**
 * We claimed to be WRITING, and the TCP socket is now writable.
 * Continue trying to complete the pending command, and then
 * move over to the the backlog.
 *
 * Returns true iff we don't leave the state.
 */
bool Session::writable()
{
    assert(!dead_);
    assert(pending_);

    if(pending_->resume()) {
	delete pending_;
	pending_ = 0;

	while(!dead_ && !backlog_.empty()) {
	    const std::string s = backlog_.front();
	    backlog_.pop();
	    Command* const cmd = command(s);
	    if(cmd->process()) {
		delete cmd; 
	    }
	    else {
		pending_ = cmd;
		break;
	    }
	}
    }

    return pending_ && !dead_;
}


/**
 * True iff the session is dead and may be deleted
 * (and removed from the epoll loop).
 */
bool Session::dead() const
{
    return dead_;
}

