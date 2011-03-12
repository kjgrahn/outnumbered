/* $Id: response.cc,v 1.4 2011-03-12 23:51:55 grahn Exp $
 *
 * Copyright (c) 2010, 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "response.h"

#if 0
#include <unistd.h>
#include <errno.h>


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
#endif
