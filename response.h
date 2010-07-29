/* -*- c++ -*-
 * $Id: response.h,v 1.2 2010-07-29 09:56:19 grahn Exp $
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

    struct CRLF {};

    template <class T>
    Response& operator<< (T&);

    ssize_t write(int fd, std::string& backlog);

private:
    Response();
    Response(const Response&);
    Response& operator= (const Response&);

    std::ostringstream oss;
    unsigned crlf;
    unsigned col;
};

#endif
