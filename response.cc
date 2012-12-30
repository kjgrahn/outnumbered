/* Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "response.h"

#include <cstring>
#include <cassert>


Response::Response()
    : text("0123456789abcdef\n"),
      done_(false)
{}


/**
 * Perform one write. Return false if further writing is needed, but
 * would probably fail if done now.  May also throw an exception,
 * e.g. at I/O error.
 */
bool Response::write(int fd)
{
    assert(!done());

    if(text) {
	done_ = filter.write(fd, Blob(text, std::strlen(text)));
	text = 0;
	return done_;
    }
    else {
	done_ = filter.write(fd);
	return done_;
    }
}


/**
 * Return true if the response is completely written.
 */
bool Response::done() const
{
    return done_;
}
