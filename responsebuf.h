/* -*- c++ -*-
 * $Id: responsebuf.h,v 1.3 2011-03-12 23:51:55 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_RESPONSEBUF_H_
#define GB_RESPONSEBUF_H_

#include <string>

namespace response {

    class Help;
    class Capabilities;
    class Date;
    class Mode;
    class Quit;

    class Group;
    class Listgroup;
    class List;

    class Article;
    class Head;
    class Body;

    class Last;
    class Next;
    class Stat;

    class Over;
    class Hdr;
    class Newnews;
    class Post1;
    class Post2;

    class Error;
}


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

    void write(const response::Help& resp);
    void write(const response::Capabilities& resp);
    void write(const response::Date& resp);
    void write(const response::Mode& resp);
    void write(const response::Quit& resp);

    void write(const response::Group& resp);
    void write(const response::Listgroup& resp);
    void write(const response::List& resp);

    void write(const response::Article& resp);
    void write(const response::Head& resp);
    void write(const response::Body& resp);

    void write(const response::Last& resp);
    void write(const response::Next& resp);
    void write(const response::Stat& resp);

    void write(const response::Over& resp);
    void write(const response::Hdr& resp);
    void write(const response::Newnews& resp);
    void write(const response::Post1& resp);
    void write(const response::Post2& resp);

    void write(const response::Error& resp);

    void write_block();
    void write_block_end();

    bool flush();

    const std::string& str() const;

    int error() const;
    const char* strerror() const;

private:
    ResponseBuf(const ResponseBuf&);
    ResponseBuf& operator=(const ResponseBuf&);
    int fd_;
};

#endif
