/* -*- c++ -*-
 * $Id: command.h,v 1.5 2011-07-03 12:21:21 grahn Exp $
 *
 * Copyright (c) 2010, 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_COMMAND_H_
#define GB_COMMAND_H_

#include <string>

class ResponseBuf;

class Command {
public:
    explicit Command(ResponseBuf& buf, const std::string& s);

    enum Type {
	UNKNOWN = 0,
	ARTICLE, BODY, CAPABILITIES, DATE,
	GROUP, HDR, HEAD, HELP,
	IHAVE, LAST,
	LIST,
	LIST_ACTIVE,
	LIST_ACTIVE_TIMES,
	LIST_DISTRIB_PATS,
	LIST_HEADERS,
	LIST_NEWSGROUPS,
	LIST_OVERVIEW_FMT,
	LISTGROUP,
	MODE_READER, NEWGROUPS, NEWNEWS, NEXT,
	OVER, POST, QUIT, STAT
    };

    static Type parse(const char* a, const char* b);

private:
    Command(const Command&);
    Command& operator= (const Command&);
};

#endif
