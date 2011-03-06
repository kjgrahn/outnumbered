/* $Id: session.cc,v 1.6 2011-03-06 23:12:21 grahn Exp $
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
	/* XXX bug: we have no way to communicating that our initial
	 * state may be anything but READING. So, we have to hope
	 * this initial short write does not block.
	 */
	pending_ = cmd;
	dead_ = true;
    }
}


Session::~Session()
{
    delete pending_;
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

    if(pending_) {
	writable();
    }
    else {
	readable();
    }

    if(dead_) return DEAD;
    return pending_? WRITING: READING;
}


/**
 * We claimed to be READING, and the TCP socket is now readable.
 *
 * The result is that:
 * - we've read all N complete lines
 * - we've processed M of them (as commands or lines of a posting)
 * and we may have moved to state WRITING, in which case:
 * - one command is left pending because we're blocked writing
 * - M lines ended up in the backlog
 * or we moved to state DEAD.
 */
void Session::readable()
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
}


Command* Session::initial()
{
    return new Command(fd_, "Hello\r\n");
}


Command* Session::command(const char* a, const char* b)
{
    return new Command(fd_, std::string(a, b));
}


Command* Session::command(const std::string& s)
{
    return command(s.data(), s.data()+s.size());
}
