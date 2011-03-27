/* $Id: responsebuf.cc,v 1.4 2011-03-27 13:15:17 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "responsebuf.h"

#include "response.h"
#include <sstream>
#include <cassert>

#include <unistd.h>
#include <errno.h>


ResponseBuf::ResponseBuf(int fd)
    : fd_(fd)
{}


bool ResponseBuf::empty() const
{
    return str().empty();
}


const std::string& ResponseBuf::str() const
{
    return str_;
}


template<class Resp>
void ResponseBuf::write(const Resp& resp)
{
    assert(empty());
    std::ostringstream oss;
    oss << resp;
    str_ = oss.str();
    flush();
}

template void ResponseBuf::write(const response::Next& resp);
template void ResponseBuf::write(const response::Quit& resp);
template void ResponseBuf::write(const response::Date& resp);
template void ResponseBuf::write(const response::Mode& resp);
template void ResponseBuf::write(const response::Group& resp);

/**
 * Try to flush the buffer into the socket. Return true iff everything
 * is flushed, otherwise keep the rest for further flushing later.
 */
bool ResponseBuf::flush()
{
    const size_t len = str_.size();
    ssize_t n = ::write(fd_, str_.data(), len);
    if(n<0) {
	return false;
    }
    if(size_t(n)==len) {
	str_.clear();
	return true;
    }
    str_.erase(0, n);
    return false;
}
