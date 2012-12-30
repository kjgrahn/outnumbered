/* -*- c++ -*-
 *
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_RESPONSE_H_
#define GB_RESPONSE_H_

#include "filter.h"

/**
 * A complete, specific HTTP response. We have already decided /how/
 * we want to respond (using which status code and so on) and all that
 * is left is to respond.
 *
 * If it hadn't been for our poll-driven/nonblocking architecture,
 * this could have been a straight function call: write headers;
 * read from file and write to socket until we're done.
 *
 * As it is, we have to be prepared for any socket write to be partial
 * or EWOULDBLOCK, in which case we have to be prepared to remember
 * our state and resume later.
 *
 */
class Response {
public:
    Response();
    bool write(int fd);
    bool done() const;

private:
    Response(const Response&);
    Response& operator= (const Response&);

    const char* text;
    bool done_;
    Filter::P filter;
};

#endif
