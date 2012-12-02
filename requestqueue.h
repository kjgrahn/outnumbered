/* -*- c++ -*-
 *
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_REQUESTQUEUE_H_
#define GB_REQUESTQUEUE_H_

#include <list>
#include <string>


/**
 * The (partial) HTTP requests of a session. Grows line by line, and
 * at any time contains zero or more complete requests, and zero or
 * one partial request.
 *
 * Knows only about high-level request syntax; interpreting requests
 * in detail is not done here.  Becomes bad() when there is no
 * complete request and its content is overly long or suspicious in
 * any other way.
 *
 * Since we don't support POST, this class is optimized for fairly
 * short requests, with no extremely long message body.
 */
class RequestQueue {
public:
    RequestQueue();

    void push(const char* a, const char* b);

    bool bad() const;
    bool complete() const;

    void pop();

private:
    RequestQueue(const RequestQueue&);
    RequestQueue& operator= (const RequestQueue&);

    typedef std::list<std::string> Lines;
    Lines buf;
};

#endif
