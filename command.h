/* -*- c++ -*-
 * $Id: command.h,v 1.6 2011-07-03 19:17:25 grahn Exp $
 *
 * Copyright (c) 2010, 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_COMMAND_H_
#define GB_COMMAND_H_

class Session;
class ResponseBuf;

namespace Command {

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

    Type parse(const char* a, const char* b);

    void initial(ResponseBuf& resp, Session& session);
    void unknown(ResponseBuf& resp, Session& session);
    void not_implemented(ResponseBuf& resp, Session& session);
}

#endif
