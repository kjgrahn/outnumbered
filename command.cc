/* $Id: command.cc,v 1.7 2011-07-03 19:40:17 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "command.h"
#include "responsebuf.h"
#include "session.h"

#include <cassert>
#include <cctype>
#include <string.h>


namespace {

    /**
     * True iff [a..b) equals the C string 's', case-insensitive
     * ASCII.
     */
    bool eq(const char* a, const char* b, const char* s)
    {
	return strncasecmp(a, s, b-a)==0;
    }

    static const char crlf[] = "\r\n";
}


/**
 * Initial parsing of a line of text [a, b) as an NNTP command.
 * Returns the command type (or variant really, since LIST is broken
 * down here) and advances 'a' past the command to the next token.
 */
Command::Type Command::parse(const char** a, const char* b)
{
    Type t = UNKNOWN;

    const char* p = *a;
    const char* q = p;
    while(q!=b && !isspace(*q)) q++;

    /* This is probably micro-optimization, but what the hell ...
     * One day I'll implement one of those fancy algorithms
     * with precalculation.
     */
    switch(q-p) {
    case 3:
	if(eq(p, q, "HDR")) t = HDR;
	break;
    case 4:
	if(eq(p, q, "BODY")) t = BODY;
	else if(eq(p, q, "DATE")) t = DATE;
	else if(eq(p, q, "HEAD")) t = HEAD;
	else if(eq(p, q, "HELP")) t = HELP;
	else if(eq(p, q, "LAST")) t = LAST;
	else if(eq(p, q, "LIST")) t = LIST;
	else if(eq(p, q, "MODE")) t = MODE_READER;
	else if(eq(p, q, "NEXT")) t = NEXT;
	else if(eq(p, q, "OVER")) t = OVER;
	else if(eq(p, q, "POST")) t = POST;
	else if(eq(p, q, "QUIT")) t = QUIT;
	else if(eq(p, q, "STAT")) t = STAT;
	break;
    case 5:
	if(eq(p, q, "GROUP")) t = GROUP;
	else if(eq(p, q, "IHAVE")) t = IHAVE;
	break;
    case 7:
	if(eq(p, q, "ARTICLE")) t = ARTICLE;
	else if(eq(p, q, "NEWNEWS")) t = NEWNEWS;
	break;
    case 9:
	if(eq(p, q, "LISTGROUP")) t = LISTGROUP;
	else if(eq(p, q, "NEWGROUPS")) t = NEWGROUPS;
	break;
    case 12:
	if(eq(p, q, "CAPABILITIES")) t = CAPABILITIES;
	break;
    default:
	break;
    }

    while(q!=b && isspace(*q)) q++;
    p = q;

    if(t==MODE_READER) {
	while(q!=b && !isspace(*q)) q++;
	if(!eq(p, q, "READER")) t = UNKNOWN;
    }
    else if(t==LIST) {
	while(q!=b && !isspace(*q)) q++;
	if(eq(p, q, "")) t = LIST;
	else if(eq(p, q, "ACTIVE")) t = LIST_ACTIVE;
	else if(eq(p, q, "ACTIVE.TIMES")) t = LIST_ACTIVE_TIMES;
	else if(eq(p, q, "DISTRIB.PATS")) t = LIST_DISTRIB_PATS;
	else if(eq(p, q, "HEADERS")) t = LIST_HEADERS;
	else if(eq(p, q, "NEWSGROUPS")) t = LIST_NEWSGROUPS;
	else if(eq(p, q, "OVERVIEW.FMT")) t = LIST_OVERVIEW_FMT;
	else t = UNKNOWN;
    }

    while(q!=b && isspace(*q)) q++;
    *a = q;

    return t;
}


void Command::initial(ResponseBuf& resp, Session&)
{
    resp << 200 << " Service available, posting allowed" << crlf;
}


void Command::unknown(ResponseBuf& resp, Session&)
{
    resp << 500 << " Unknown command" << crlf;
}


void Command::not_implemented(ResponseBuf& resp, Session&)
{
    resp << 503 << " Feature not supported" << crlf;
}
