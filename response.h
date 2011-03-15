/* -*- c++ -*-
 * $Id: response.h,v 1.5 2011-03-15 22:03:48 grahn Exp $
 *
 * Copyright (c) 2010, 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_RESPONSE_H_
#define GB_RESPONSE_H_

#include <iosfwd>

#include <string>

#include "number.h"
#include "msgid.h"


namespace response {

    struct Help {
	Help() {}
	void put(std::ostream& os) const;
    };

    struct Capabilities {
	template<class Iter>
	explicit Capabilities(Iter begin, Iter end);
	void put(std::ostream& os) const;
    };

    struct Date {
	explicit Date(time_t t)
	    : t(t)
	{}
	void put(std::ostream& os) const;
	const time_t t;
    };
    inline std::ostream& operator<< (std::ostream& os, const Date& val) {
	val.put(os); return os;
    }

    struct Mode {
	Mode(bool posting_ok, const char* msg)
	    : posting_ok(posting_ok),
	      msg(msg)
	{}
	void put(std::ostream& os) const;
	bool posting_ok;
	const char* msg;
    };
    inline std::ostream& operator<< (std::ostream& os, const Mode& val) {
	val.put(os); return os;
    }

    struct Quit {
	Quit(const char* msg)
	    : msg(msg)
	{}
	void put(std::ostream& os) const;
	const char* msg;
    };
    inline std::ostream& operator<< (std::ostream& os, const Quit& val) {
	val.put(os); return os;
    }

    struct Group {
	explicit Group(unsigned n,
		       unsigned low, unsigned high,
		       const char* group)
	    : n(n),
	      low(low),
	      high(high),
	      group(group)
	{}
	void put(std::ostream& os) const;
	unsigned n;
	unsigned low;
	unsigned high;
	const char* group;
    };
    inline std::ostream& operator<< (std::ostream& os, const Group& val) {
	val.put(os); return os;
    }

    struct Listgroup {
	explicit Listgroup(unsigned n,
			   unsigned low, unsigned high,
			   const char* group);
	void put(std::ostream& os) const;
    };
    inline std::ostream& operator<< (std::ostream& os, const Listgroup& val) {
	val.put(os); return os;
    }

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
	Next(Number n, const MsgId& msgid) : n(n), msgid(msgid) {}
	void put(std::ostream& os) const;
	const Number n;
	const MsgId& msgid;
    };
    inline std::ostream& operator<< (std::ostream& os, const Next& val) {
	val.put(os); return os;
    }

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
