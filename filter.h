/* -*- c++ -*-
 *
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_FILTER_H_
#define GB_FILTER_H_

#include "blob.h"
#include "deflate.h"

#include <vector>


/**
 * Slightly funny name for a thing used to write to a nonblocking
 * socket, and which stores unwritten parts until the next write.
 */
class Backlog {
public:
    Backlog() {}
    bool empty() const { return v.empty(); }
    size_t write(int fd);
    size_t write(int fd, const Blob& a);
    size_t write(int fd, const Blob& a, const Blob& b);
    size_t write(int fd, const Blob& a, const Blob& b, const Blob& c);

    typedef std::vector<char> Buf;

private:
    Backlog(const Backlog&);
    Backlog& operator= (const Backlog&);

    Buf v;
};


/**
 * Response writing filters.
 * These are chainable, because you can write:
 * (a) headers
 * (b) data (entity-body) as-is
 * (c) data compressed
 * (d) chunked data
 * (e) compressed and chunked data
 *
 * The thing is, what's at the bottom is a nonblocking socket
 * which may only partially accept a write(), but we need something
 * inbetween which will buffer the remainder *and* at the same time
 * signal the original writer to back off until the socket becomes
 * writable again.
 *
 * bool write(fd, data) has the semantics:
 * true:  all data was successfully written to the fd, the backlog
 *        is empty, and you may continue writing
 * false: some data was /not/ written successfully, and was stored
 *        in the backlog. You may not write again unless the fd goes
 *        writable
 * All other situations map to an exception being thrown.
 *
 */
namespace Filter {

    /**
     * Plain or identity filter; write data verbatim.
     */
    class Plain {
    public:
	Plain() {}
	bool write(int fd) {
	    return backlog.write(fd)==0;
	}
	bool write(int fd, const Blob& a) {
	    return backlog.write(fd, a)==0;
	}
	bool write(int fd, const Blob& a, const Blob& b) {
	    return backlog.write(fd, a, b)==0;
	}
	bool write(int fd, const Blob& a, const Blob& b, const Blob& c) {
	    return backlog.write(fd, a, b, c)==0;
	}

    private:
	Plain(const Plain&);
	Plain& operator= (const Plain&);

	Backlog backlog;
    };


    /**
     * Write using chunked transfer coding [RFC 2616 3.6.1].
     */
    template<class Next>
    class Chunked {
    public:
	Chunked() {}
	bool write(int fd) { return next.write(fd); }
	bool write(int fd, const Blob& a);
	bool write(int fd, const Blob& a, const Blob& b);

    private:
	Chunked(const Chunked&);
	Chunked& operator= (const Chunked&);

	Next next;
    };


    /**
     * Write using zlib compression [RFC 2616 3.5, 3.6].
     *
     * Unlike the other filters this one has an end() function, which
     * must be used to end the input. Otherwise it behaves as any of
     * the write() methods; you may have to keep calling write(fd)
     * afterwards.
     */
    template<class Next>
    class Zlib {
    public:
	Zlib() {}
	bool write(int fd);
	bool write(int fd, const Blob& a);
	bool write(int fd, const Blob& a, const Blob& b);
	bool end(int fd);

    private:
	Zlib(const Zlib&);
	Zlib& operator= (const Zlib&);

	Next next;
	Deflate compress;
    };


    /* shortcuts */
    typedef Plain P;
    typedef Zlib<Plain> Z;
    typedef Chunked<Plain> C;
    typedef Zlib<C> CZ;
}

#endif
