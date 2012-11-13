/*
 * Copyright (c) 2010, 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "textread.h"

#include <algorithm>
#include <cstring>

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>


using namespace sockutil;


TextReader::TextReader(const std::string& endline)
    : endline_(endline),
      p_(buf),
      q_(p_+sizeof buf),
      a_(p_),
      b_(p_),
      eof_(false),
      errno_(0)
{}


TextReader::TextReader(const TextReader& other)
    : endline_(other.endline_),
      p_(buf),
      q_(p_+sizeof buf),
      a_(p_),
      b_(p_+(other.b_-other.a_)),
      eof_(other.eof_),
      errno_(other.errno_)
{
    std::copy(other.a_, other.b_, a_);
}      


TextReader& TextReader::operator= (const TextReader& other)
{
    endline_ = other.endline_;
    a_ = p_;
    b_ = p_ + (other.b_-other.a_);
    eof_ = other.eof_;
    errno_ = other.errno_;
    if(other.a_!=a_) std::copy(other.a_, other.b_, a_);
    return *this;
}


void TextReader::feed(int fd)
{
    if(a_!=b_ && a_!=p_) {
	std::copy(a_, b_, p_);
    }
    b_ -= (a_-p_);
    a_ = p_;

    const ssize_t n = recv(fd, b_, q_-b_, 0);
    if(n==-1) {
	switch(errno) {
	case EAGAIN:
	case EINTR:
	    break;
	default:
	    eof_ = true;
	    errno_ = errno;
	}
    }
    else if(n==0) {
	eof_ = true;
    }
    else {
	b_+=n;
    }
}


size_t TextReader::read(char*& begin, char*& end)
{
    char* const c = std::search(a_, b_,
				endline_.begin(), endline_.end());
    if(c==b_) {
	if(eof_) {
	    /* The last of the wine ... return it even without
	     * an endline marker.
	     *
	     * XXX Slight loophole here -- eof_ may be a false one
	     * caused by an overlong line. But we shouldn't have
	     * kept reading in that case.
	     */
	    begin = a_;
	    end = b_;
	    a_ = b_;
	    return (end-begin);
	}

	if(a_==p_ && b_==q_) {
	    /* The buffer is full and yet there is no endline, i.e.  we've
	     * found a line that's too long to read.  We could read it as
	     * a fragment, but for now choose the other option: treat it
	     * as an error.
	     */
	    eof_ = true;
	}

	return 0;
    }

    begin = a_;
    end = c + endline_.size();
    a_ = end;

    return (end-begin);
}


std::string TextReader::read()
{
    char* a = 0;
    char* b = 0;
    read(a, b);
    return std::string(a, b);
}


/**
 * Same as strerror(error()).
 */
const char* TextReader::strerror() const
{
    return std::strerror(error());
}
