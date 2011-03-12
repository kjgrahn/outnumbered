/* -*- c++ -*-
 * $Id: response.h,v 1.4 2011-03-12 23:51:55 grahn Exp $
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

#include "number.h"
#include "msgid.h"


namespace response {

    struct Help {
	explicit Help();
    };

    struct Capabilities {
	template<class Iter>
	explicit Capabilities(Iter begin, Iter end);
    };

    struct Date {
	explicit Date(time_t);
    };

    struct Mode {
	Mode(bool posting_ok, const char* msg);
    };

    struct Quit {
	Quit(const char* msg);
    };

    struct Group {
	explicit Group(unsigned n,
		       unsigned low, unsigned high,
		       const char* group);
    };

    struct Listgroup {
	explicit Listgroup(unsigned n,
			   unsigned low, unsigned high,
			   const char* group);
    };

    struct List {
	explicit List();
    };

    struct Article {
	explicit Article(Number n, const MsgId& msgid);
    };

    struct Head {
	explicit Head(Number n, const MsgId& msgid);
    };

    struct Body {
	explicit Body(Number n, const MsgId& msgid);
    };

    struct Last {
	explicit Last();
    };

    struct Next {
	Next(Number n, const MsgId& msgid);
    };

    struct Stat {
	explicit Stat();
    };

    struct Over {
	explicit Over();
    };

    struct Hdr {
	explicit Hdr();
    };

    struct Newnews {
	explicit Newnews();
    };

    struct Post1 {
	explicit Post1();
    };

    struct Post2 {
	explicit Post2();
    };
}

#endif
