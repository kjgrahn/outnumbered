/* $Id: response.cc,v 1.6 2011-03-16 21:23:19 grahn Exp $
 *
 * Copyright (c) 2010, 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "response.h"

#include <iostream>
#include <cassert>
#include <time.h>


using namespace response;
using std::ostream;

namespace {
    static const char crlf[] = "\r\n";
}


std::ostream& operator<< (std::ostream& os, const Number& val)
{
    return os << val.val;
}

std::ostream& operator<< (std::ostream& os, const MsgId& val)
{
    return os << '<' << val.val << '>';
}


void Help::put(ostream& os) const
{
    os << "666 foo" << crlf;
}

void Capabilities::put(ostream& os) const
{
    os << "666 foo" << crlf;
}

void Date::put(ostream& os) const
{
    const struct tm* time = gmtime(&t);
    assert(time);
    char buf[3+1+4+5*2+2+1];
    size_t n = strftime(buf, sizeof buf,
			"111 %Y%m%d%H%M%S\r\n", time);
    assert(n);
    os << buf;
}

void Mode::put(ostream& os) const
{
    os << (posting_ok ? "200 " : "201 ")
       << msg << crlf;
}

void Quit::put(ostream& os) const
{
    os << "205 " << msg << crlf;
}

void Group::put(ostream& os) const
{
    os << "211 "
       << n << ' '
       << low << ' ' << high << ' '
       << group << " selected" << crlf;
}

void Listgroup::put(ostream& os) const
{
    os << "666 foo" << crlf;
}

void Next::put(ostream& os) const
{
    os << "223 " << n << ' ' << msgid << crlf;
}
