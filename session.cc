/* $Id: session.cc,v 1.2 2010-07-28 12:39:37 grahn Exp $
 *
 * Copyright (c) 2010 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "client.h"

#include "response.h"

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
      reader_(sockutil::TextReader(fd, "\r\n")),
      peer_(getnameinfo(sa))
{
    const char* welcome = "welcome,";
    Response r(200); r << welcome << peer_;
    r.write(fd_, backlog_);
}


Client::~Client()
{
    close(fd_);
}


/**
 * The TCP socket is readable; try to do some useful work.
 */
void Client::feed()
{
    reader_.feed();

    char* a;
    char* b;
    while(reader_.read(a, b)) {
	read(a, b);
    }
}


void Client::read(const char* a, const char* b)
{
    std::cerr << peer_ << " - " << std::string(a, b);
}


/**
 * To be called after feed().
 */
bool Client::eof() const
{
    return reader_.eof();
}
