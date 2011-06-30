/* -*- c++ -*-
 * $Id: responsebuf.h,v 1.6 2011-06-30 09:15:29 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_RESPONSEBUF_H_
#define GB_RESPONSEBUF_H_

#include <string>
#include <ostream>

/**
 * Writing NNTP responses to a non-blocking stream socket,
 * and buffering them if EWOULDBLOCK for later flushing.
 *
 * - The ostream interface only queues up text, and never causes an
 *   actual socket write. The text must include CRLF pairs
 *   (ResponseBuf does no automatic conversion from '\n' to CRLF) but
 *   must not do dot-stuffing (ResponseBuf /does/ handle that).
 *
 * - write_termination() writes the termination line of a multi-line
 *   response; the line with just a '.'. That can't be done via the
 *   ostream, since that interface is subject to dot-stuffing.
 *
 * - flush() starts the actual socket writing. This must be done when
 *   there are no half-complete responses in the queue, and must be
 *   repeated until the queue is empty. The ostream meanwhile must not
 *   be used.
 *
 * This way of handling writes assumes that there's an upper bound on
 * the amount of data to write; it wouldn't be suitable for streaming
 * gigabyte-sized chunks of data. Fortunately, NNTP is more dialogue-
 * oriented and an article or a search result is not very large.
 *
 * The intent behind the buffering is to let any NNTP command complete
 * atomically, i.e. without involving the main select loop. When the full
 * response has hit the ResponseBuf, the command is done.
 */
class ResponseBuf {
public:
    explicit ResponseBuf(int fd);
    ~ResponseBuf();

    bool empty() const;
    size_t size() const;

    std::ostream& ostream() { return os_; }
    void write_termination();

    bool flush();
    int error() const;
    const char* strerror() const;

    std::string str() const;

private:
    ResponseBuf(const ResponseBuf&);
    ResponseBuf& operator=(const ResponseBuf&);
    int fd_;

    class StreamBuf;
    StreamBuf* const buf_;
    std::ostream os_;
};

template <class T>
inline
std::ostream& operator<< (ResponseBuf& buf, const T& t)
{
    return buf.ostream() << t;
}

#endif
