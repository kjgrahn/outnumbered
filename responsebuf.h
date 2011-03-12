/* -*- c++ -*-
 * $Id: responsebuf.h,v 1.1 2011-03-12 09:43:29 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_RESPONSEBUF_H_
#define GB_RESPONSEBUF_H_

/**
 * Writing NNTP responses to a non-blocking stream socket,
 * and buffering them if EWOULDBLOCK for later flushing.
 *
 * The interface is suited for writing NNTP responses (including
 * multi-line blocks [RFC 3977 3.1.3]). Initially the writes go
 * straight to the fd, but if EWOULDBLOCK happens, it starts
 * accumulating to an internal buffer instead.  Then you flush()
 * repeatedly -- preferrably when the socket becomes writable -- until
 * at last the buffer is emptied.
 *
 * This way of handling writes assumes that there's an upper bound on
 * the amount of data to write; it wouldn't be suitable for streaming
 * gigabyte-sized chunks of data. Fortunately, NNTP is more dialogue-
 * oriented and an article or a search result is not very large.
 *
 * The intent behind the buffering is to let any NNTP command complete
 * atomically, i.e. without involving the main select loop.
 */
class ResponseBuf {
public:
    explicit ResponseBuf(int fd);
    bool empty() const;

    int error() const;
    const char* strerror() const;

private:
    ResponseBuf(const ResponseBuf&);
    ResponseBuf& operator=(const ResponseBuf&);
    int fd_;
};

#endif
