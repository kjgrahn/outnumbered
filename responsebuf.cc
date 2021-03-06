/* $Id: responsebuf.cc,v 1.14 2011-07-03 08:27:21 grahn Exp $
 *
 * Copyright (c) 2011 J�rgen Grahn
 * All rights reserved.
 *
 */
#include "responsebuf.h"

#include <algorithm>
#include <cassert>

#include <unistd.h>
#include <errno.h>


/**
 * The actual streambuf is implemented in an infinitely growing
 * vector<char> where the buffering takes place:
 *
 * v.begin()                                   v.end()
 * pbase             z            pptr         epptr
 * |:::::::::::::::::|::::::::::::|............|
 *
 * When queueing, 'z' marks the end of dot-stuffed data; [z, pptr) contains no
 * complete multi-line responses.
 *
 * When flushing, [pbase .. pptr) is the area which remains
 * to write.
 */


ResponseBuf::ResponseBuf()
    : vec_(4096),
      os_(this)
{
    clear_buf();
}


ResponseBuf::~ResponseBuf()
{}


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
void ResponseBuf::put_terminator()
{
    const bool add_extra_crlf = !is_crlf_terminated(pbase(), pptr());
    size_t dots = count_dots(z_, pptr());
    const size_t growth = dots + add_extra_crlf? 2+3: 3;

    if(pptr()+growth > epptr()) grow_buf();
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


/**
 * Reset the StreamBuf, so it's fully usable for queuing again.
 * The vector is not resized.
 */
void ResponseBuf::clear_buf()
{
    char_type* p = &vec_[0];
    setp(p, p + vec_.size());
    z_ = p;
}


ResponseBuf::int_type ResponseBuf::overflow(ResponseBuf::int_type c)
{
    grow_buf();
    sputc(c);
    return '!';
}


/**
 * Grow the buffer with 50% or so, i.e. with at least 2K.
 */
void ResponseBuf::grow_buf()
{
    const size_t dz = z_ - pbase();
    const size_t dp = pptr() - pbase();

    vec_.resize(vec_.size()*3/2);
    char_type* const p = &vec_[0];
    setp(p, p + vec_.size());
    z_ = p + dz;
    pbump(dp);
}


/**
 * Pull out the contents of the buffer as a string.
 * For test purposes only.
 */
std::string ResponseBuf::str() const
{
    return std::string(pbase(), pptr());
}


/**
 * Try to flush the buffer into 'fd', by performing exactly one
 * write(2). Follows standard write(2) conventions for return code and
 * setting of errno, except the return code says how many bytes remain
 * /unwritten/:
 *
 * -1 : nothing written; check errno
 *  0 : wrote ok; buffer is now empty
 *  N : wrote ok, but N octets remain
 *
 * Thus you stop writing when it returns 0, or it returns -1 and errno
 * says something fatal happened.
 *
 * When the buffer is completely flushed, it's automatically available
 * for writing responses into again.
 */
int ResponseBuf::flush(int fd)
{
    /* a-----a'------b�����c
     *    n
     */
    char_type* a = pbase();
    char_type* b = pptr();
    char_type* c = epptr();

    ssize_t n = ::write(fd, a, b-a);
    if(n<0) {
	return n;
    }

    a += n;
    if(a==b) {
	clear_buf();
	return 0;
    }
    setp(a, c);
    pbump(b-a);

    return b - a;
}
