/* $Id: responsebuf.cc,v 1.11 2011-07-02 08:42:59 grahn Exp $
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

    void put_terminator();

protected:
    virtual int_type overflow(int_type c = traits_type::eof());

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

    static const char dot[] = "\r\n.";
    static const char* const dotend = dot + sizeof(dot)-1;
    static const char term[] = ".\r\n";
    static const char* const termend = term + sizeof(term)-1;
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
     * Find 'dot' backwards in [a, p) and returns a pointer
     * to the start of 'dot', which is assumed to exist.
     */
    char* find_dot(char* a, char* b)
    {
	char* p = std::find_end(a, b, dot, dotend);
	assert(p!=b);
	return p;
    }

    /**
     * True if [a, b) is CRLF-terminated.
     */
    bool is_crlf_terminated(const char* a, const char* b)
    {
	return (b-a >= crlfend - crlf) &&
	    std::equal(b-(crlfend - crlf), b, crlf);
    }

    /**
     * Move [a, b) right by n positions.
     */
    void move_right(char* a, char* b, size_t n)
    {
	std::copy_backward(a, b, b+n);
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
void ResponseBuf::StreamBuf::put_terminator()
{
    const bool add_extra_crlf = !is_crlf_terminated(pbase(), pptr());
    size_t dots = count_dots(z_, pptr());
    const size_t growth = dots + add_extra_crlf? 2+3: 3;

    if(pptr()+growth > epptr()) grow();
    assert(!(pptr()+growth > epptr()));

    char_type* p = pptr();
    pbump(dots);
    while(dots) {
	char_type* q = find_dot(z_, p);
	move_right(q+3, p, dots);
	*(q+3) = '.';
	dots--;
	move_right(q, q+4, dots);
	p = q;
    }

    p = pptr();
    if(add_extra_crlf) {
	std::copy(crlf, crlfend, p);
	p += crlfend - crlf;
    }
    std::copy(term, termend, p);
    p += termend - term;
    pbump(p - pptr());
    z_ = pptr();
}


ResponseBuf::StreamBuf::int_type
ResponseBuf::StreamBuf::overflow(ResponseBuf::StreamBuf::int_type c)
{
    grow();
    sputc(c);
    return '!';
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


void ResponseBuf::put_terminator()
{
    buf_->put_terminator();
}


size_t ResponseBuf::size() const
{
    return buf_->size();
}


/**
 * Pull out the contents of the buffer as a string.
 * For test purposes only.
 */
std::string ResponseBuf::str() const
{
    return buf_->str();
}


#if 0
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
