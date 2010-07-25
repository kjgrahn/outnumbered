/* $Id: session.cc,v 1.1 2010-07-25 20:20:35 grahn Exp $
 *
 * Copyright (c) 2010 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "client.h"

#include <iostream>
#include <sstream>

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


Client::Client(int fd, const sockaddr_storage& sa)
    : fd_(fd),
      reader_(new sockutil::TextReader(fd, "\r\n")),
      peer_(getnameinfo(sa))
{}


Client::~Client()
{
    delete reader_;
    if(fd_!=-1) {
	close(fd_);
    }
}


/**
 * The TCP socket is readable; try to do some useful work.
 */
void Client::feed()
{
    sockutil::TextReader& reader = *reader_;
    reader.feed();

    char* a;
    char* b;
    while(reader.read(a, b)) {
	std::cerr << peer_ << " - " << std::string(a, b);
    }
}


/**
 * To be called after feed().
 */
bool Client::eof() const
{
    return reader_->eof();
}
