/* -*- c++ -*-
 * $Id: response.h,v 1.3 2010-07-29 11:51:52 grahn Exp $
 *
 * Copyright (c) 2010 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_RESPONSE_H_
#define GB_RESPONSE_H_

#include <iostream>
#include <sstream>

#include <string>

struct CrLf {};
static const CrLf CRLF();

/**
 * An NNTP response, e.g.
 *     211 1234 3000234 3002322 misc.test
 * or
 *     215 list of newsgroups follows
 *     misc.test 3002322 3000234 y
 *     comp.risks 442001 441099 m
 *     alt.rfc-writers.recovery 4 1 y
 *     tx.natives.recovery 89 56 y
 *     tx.natives.recovery.d 11 9 n
 *     .
 * 
 * Takes care of inserting spaces, some CRLF and the final dot.
 * - for multi-line responses you must end each line manually with CRLF,
 *   but the final dot line is added automatically.
 * - for single-line responses you must not add the CRLF.
 *
 * Also takes care of dot-stuffing, but only when you << a char* or a
 * std::string. These must not contain \n either -- you are supposed
 * to use this class for simple structured responses, not for sending
 * full articles.
 */
class Response {
public:
    explicit Response(unsigned code);

    template <class T>
    Response& operator<< (T val);

    ssize_t write(int fd, std::string& backlog);
    std::string str() const;

private:
    Response();
    Response(const Response&);
    Response& operator= (const Response&);

    void finalize();

    std::ostringstream oss;
    unsigned crlf;
    unsigned col;
};


template <class T>
Response& Response::operator<< (T val)
{
    if(col++) oss << ' ';
    oss << val;
    return *this;
}

template <>
Response& Response::operator<< (const CrLf&);


#endif
