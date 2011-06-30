/* $Id: responsebuf.cc,v 1.6 2011-06-30 21:37:49 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "responsebuf.h"

#include <iostream>
#include <streambuf>
#include <vector>
#include <algorithm>
#include <cassert>

#include <unistd.h>
#include <errno.h>


/**
 * The actual streambuf. Owns an infinitely growing vector<char> where the buffering
 * takes place:
 *
 * pbase             z            pptr         epptr,size
 * |:::::::::::::::::|::::::::::::|............|
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
    StreamBuf(const StreamBuf&);
    StreamBuf& operator= (const StreamBuf&);

    void grow();

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


namespace {

    static const char dot[] = ".\r\n";
    static const char* const dotend = dot + sizeof(dot)-1;
    static const char crlf[] = "\r\n";
    static const char* const crlfend = crlf + sizeof(crlf)-1;

    /**
     * The number of occurences of 'dot' in [a, b).
     */
    int count_dots(const char* a, const char* b)
    {
	int n = 0;
	while((a = std::search(a, b, dot, dotend)) != b) {
	    a += dotend-dot;
	    ++n;
	}
	return n;
    }

    /**
     * True if [a, b) is CRLF-terminated.
     */
    bool is_crlf_terminated(const char* a, const char* b)
    {
	return (b-a >= crlfend - crlf) &&
	    std::equal(b-(crlfend - crlf), b, crlf);
    }

}


/**
 * Write the multi-line response terminator ".\r\n".
 * Also terminates the line before it, if necessary.
 * Also, performs dot-stuffing of the buffer between
 * this point and the previous terminator, and advances z_.
 *
 * May have to grow() the buffer either because of the terminator
 * itself or the dot-stuffing. Fortunately, a single grow() is always
 * enough; dot-stuffing forces worst-case 33% growth.
 */
void ResponseBuf::StreamBuf::write_termination()
{
    const bool add_extra_crlf = !is_crlf_terminated(pbase(), pptr());
    size_t growth = count_dots(z_, pptr());
    growth += add_extra_crlf? 2+3: 3;

    if(pptr()+growth > epptr()) grow();
    assert(!(pptr()+growth > epptr()));

    char_type* p = pptr();
    if(add_extra_crlf) {
	std::copy(crlf, crlfend, p);
	p += crlfend - crlf;
    }
    std::copy(dot, dotend, p);
    p += dotend - dot;
    pbump(p - pptr());
}


/**
 * Grow the buffer with 50% or so, i.e. with at least 2K.
 */
void ResponseBuf::StreamBuf::grow()
{
    const size_t dz = z_ - pbase();
    const size_t dp = pptr() - pbase();

    vec_.resize(vec_.size()*3/2);
    char_type* const p = &vec_[0];
    setp(p, p + vec_.size());
    z_ = p + dz;
    pbump(dp);
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
