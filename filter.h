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


class Backlog {
public:
    bool empty() const;
    Blob buf() const;

private:
    Backlog(const Backlog&);
    Backlog& operator= (const Backlog&);
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
	Plain();
	bool write(int fd);
	bool write(int fd, const Blob& a);
	bool write(int fd, const Blob& a, const Blob& b);

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
	Chunked();
	bool write(int fd);
	bool write(int fd, const Blob& a);
	bool write(int fd, const Blob& a, const Blob& b);

    private:
	Chunked(const Chunked&);
	Chunked& operator= (const Chunked&);

	Next next;
    };


    /**
     * Write using zlib compression [RFC 2616 3.5, 3.6].
     */
    template<class Next>
    class Zlib {
    public:
	Zlib();
	bool write(int fd);
	bool write(int fd, const Blob& a);
	bool write(int fd, const Blob& a, const Blob& b);

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
