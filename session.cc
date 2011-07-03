/* $Id: session.cc,v 1.9 2011-07-03 10:12:37 grahn Exp $
 *
 * Copyright (c) 2010, 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "session.h"

#include "command.h"

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
      reader_(fd, "\r\n"),
      peer_(getnameinfo(sa)),
      dead_(false),
      posting_(0)
{
    initial();
    flush();
    if(!writer_.empty()) {
	/* XXX bug: we have no way to communicating that our initial
	 * state may be anything but READING. So, we have to hope
	 * this initial short write does not block.
	 */
	dead_ = true;
    }
}


Session::~Session()
{
    delete posting_;
    close(fd_);
}


/**
 * There's an event on the fd; process it. Since the caller only listens for
 * readability *or* writability and it does it according to what it was told
 * about our internal state, we know which event it was.
 */
Session::State Session::event()
{
    assert(!dead_);

    if(!writer_.empty()) {
	writable();
    }
    else {
	readable();
    }

    if(dead_) return DEAD;
    return writer_.empty() ? READING : WRITING;
}


/**
 * We claimed to be READING, and the TCP socket is now readable.
 *
 * The result is that:
 * - we've read all N complete lines
 * - we've processed M of them (as commands or lines of a posting)
 * and we may have moved to state WRITING, in which case:
 * - the ResponseBuf isn't empty; it contains the tail of a response
 *   because we'd block if we tried writing it
 * - M input lines ended up in the backlog
 * or we moved to state DEAD.
 */
void Session::readable()
{
    assert(!dead_);
    assert(writer_.empty());
    assert(backlog_.empty());

    reader_.feed();

    char* a;
    char* b;
    while(!dead_ && reader_.read(a, b)) {

	command(a, b);
	flush();
	if(!writer_.empty()) {
	    break;
	}
    }

    if(!writer_.empty()) {
	while(reader_.read(a, b)) {
	    backlog_.push(std::string(a, b));
	}
    }

    /* XXX Too strict; what if we have stuff to write? */
    dead_ |= reader_.eof();
}


/**
 * We claimed to be WRITING, and the TCP socket is now writable.
 *
 * Continue trying to complete the pending command, and then
 * move over to the the backlog.
 */
void Session::writable()
{
    assert(!dead_);
    assert(!writer_.empty());

    flush();

    if(writer_.empty()) {
	/* Ok, so the flushing of pending responses is complete, but
	 * we may have a backlog of commands, and need to handle them
	 * before reading.
	 */
	while(!dead_ && !backlog_.empty()) {
	    const std::string s = backlog_.front();
	    backlog_.pop();
	    command(s);
	    flush();
	    if(!writer_.empty()) {
		break;
	    }
	}
    }
}


/**
 * Try to empty the ResponseBuf to the socket by making one
 * flush() attempt. Doesn't change state, except it may set dead_
 * in case of a fatal I/O error.
 */
void Session::flush()
{
    assert(!dead_);
    if(writer_.empty()) return;

    const int n = writer_.flush(fd_);
    if(n==-1) {
	switch(errno) {
	case EAGAIN:
	case EINTR:
	    break;
	default:
	    std::cerr << peer_ << ": write failed: " << strerror(errno) << '\n';
	case ECONNRESET:
	case EPIPE:
	    std::cerr << peer_ << ": closing\n";
	    dead_ = true;
	    break;
	}
    }
}


void Session::initial()
{
    Command(writer_, "");
}


void Session::command(const char* a, const char* b)
{
    Command(writer_, std::string(a, b));
}


void Session::command(const std::string& s)
{
    command(s.data(), s.data()+s.size());
}
