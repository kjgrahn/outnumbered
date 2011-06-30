/* $Id: responsebuf.cc,v 1.5 2011-06-30 09:15:29 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "responsebuf.h"

#include <streambuf>
#include <vector>
#include <cassert>

#include <unistd.h>
#include <errno.h>


/**
 * The actual streambuf. Owns an infinitely growing vector<char> where the buffering
 * takes place:
 *
 * pbase             z            pptr         epptr,size     capacity 
 * |:::::::::::::::::|::::::::::::|............|..............|
 *
 * When queueing, 'z' marks the end of dot-stuffed data; [z, pptr) contains no
 * complete multi-line responses.
 */
class ResponseBuf::StreamBuf : public std::basic_streambuf<char> {
public:
    typedef std::vector<char_type> Vec;

    StreamBuf();

    size_t size() const { return pptr() - pbase(); }
    bool empty() const { return size()==0; }
    std::string str() const { return std::string(pbase(), pptr()); }

    void write_termination();

private:
    Vec vec_;
    char_type* z_;
};


ResponseBuf::StreamBuf::StreamBuf()
    : vec_(4096)
{
    char_type* p = &vec_[0];
    setp(p, p + vec_.size());
    z_ = p;
}


void ResponseBuf::StreamBuf::write_termination()
{
#if 0
    assert(size()>2);
    char_type* p = pend()-2;
    if(p[0]!='\r' || p[1]!='\n') {
    }
#endif
}


ResponseBuf::ResponseBuf(int fd)
    : fd_(fd),
      buf_(new ResponseBuf::StreamBuf),
      os_(buf_)
{}


ResponseBuf::~ResponseBuf()
{
    delete buf_;
}


bool ResponseBuf::empty() const
{
    return buf_->empty();
}


void ResponseBuf::write_termination()
{
    buf_->write_termination();
}


size_t ResponseBuf::size() const
{
    return buf_->size();
}


std::string ResponseBuf::str() const
{
    return buf_->str();
}


#if 0
template<class Resp>
void ResponseBuf::write(const Resp& resp)
{
    assert(empty());
    std::ostringstream oss;
    oss << resp;
    str_ = oss.str();
    flush();
}


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
#endif
