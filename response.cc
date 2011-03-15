/* $Id: response.cc,v 1.5 2011-03-15 22:03:48 grahn Exp $
 *
 * Copyright (c) 2010, 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "response.h"

#include <iostream>

using namespace response;
using std::ostream;

namespace {
    static const char crlf[] = "\r\n";
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
    os << "666 foo" << crlf;
}

void Mode::put(ostream& os) const
{
    os << "666 foo" << crlf;
}

void Quit::put(ostream& os) const
{
    os << "666 foo" << crlf;
}

void Group::put(ostream& os) const
{
    os << "666 foo" << crlf;
}

void Listgroup::put(ostream& os) const
{
    os << "666 foo" << crlf;
}

void Next::put(ostream& os) const
{
    os << "666 foo" << crlf;
}
