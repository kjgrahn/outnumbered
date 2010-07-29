/* $Id: response.cc,v 1.2 2010-07-29 11:51:52 grahn Exp $
 *
 * Copyright (c) 2010 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "response.h"

#include <unistd.h>
#include <errno.h>


Response::Response(unsigned code)
    : crlf(0),
      col(1)
{
    oss << code;
}


template <>
Response& Response::operator<< (const CrLf&)
{
    oss << "\r\n";
    ++crlf;
    col = 0;
    return *this;
}


template <>
Response& Response::operator<< (const char*& s)
{
    if(col++) {
	oss << ' ';
    }
    else if(crlf && s[0]=='.') {
	oss << '.';
    }
    oss << s;
    return *this;
}


template <>
Response& Response::operator<< (const std::string& s)
{
    if(col++) {
	oss << ' ';
    }
    else if(crlf && !s.empty() && s[0]=='.') {
	oss << '.';
    }
    oss << s;
    return *this;
}


void Response::finalize()
{
    if(crlf) {
	oss << ".\r\n";
    }
    else {
	oss << "\r\n";
    }
}


/**
 * Like write(2) of the content (after finalizing it). Returns what
 * write(2) returns, and leaves its errno intact.
 *
 * In case of a short write or EAGAIN, the remaining text is placed in
 * 'backlog'.
 */
ssize_t Response::write(const int fd, std::string& backlog)
{
    finalize();
    std::string s = oss.str();
    const ssize_t n = ::write(fd, s.data(), s.size());
    if((n==-1 && errno==EAGAIN) ||
       (n>=0 && unsigned(n) != s.size())) {
	s.erase(0, n);
	std::swap(s, backlog);
    }
    return n;
}


Response::Response(const Response& other)
    : oss(other.oss.str()),
      crlf(other.crlf),
      col(other.col)
{}


std::string Response::str() const
{
    Response copy(*this);
    copy.finalize();
    return copy.oss.str();
}
