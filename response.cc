/* Copyright (c) 2012, 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "response.h"
#include <cassert>
#include <cctype>

namespace {

    std::string rstrip(const std::string& s)
    {
	std::string r = s;

	const char* const p = s.c_str();
	const char* q = p + s.size();
	while(p!=q) {
	    if(!isspace(*(q-1))) break;
	    q--;
	}
	r.resize(q-p);
	return r;
    }
}


Response::Response(const std::string& request)
    : src(rstrip(request)),
      done_(false)
{}


/**
 * Perform one write. Return false if further writing is needed, but
 * would probably fail if done now.  May also throw an exception,
 * e.g. at I/O error.
 */
bool Response::write(int fd)
{
    /* When we get here, we may be in either of several
     * valid states:
     *
     * src    filter     effect
     * ------ ------     ------
     * 	 data      -     write data to filter
     * 	 data   data     write data to filter
     * 	    -      -     write eof
     * 	    -   data     write eof
     * 	    -   data+eof flush
     */

    assert(!done());

    if(src.alive()) {
	char buf[8*1024];
	size_t n = src.read(buf, sizeof buf);
	if(n) {
	    return filter.write(fd, Blob(buf, n));
	}
    }

    return done_ = filter.end(fd);
}


/**
 * Return true if the response has been completed (and can be
 * destroyed).
 */
bool Response::done() const
{
    return done_;
}
