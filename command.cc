/* $Id: command.cc,v 1.5 2011-07-03 12:59:09 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "command.h"
#include "responsebuf.h"
#include "response.h"

#include <cassert>
#include <unistd.h>
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
}


/**
 * Initial parsing of a line of text as an NNTP command.
 */
Command::Type Command::parse(const char* a, const char* b)
{
    Type t = UNKNOWN;

    const char* p = a;
    while(p!=b && !isspace(*p)) p++;

    /* This is probably micro-optimization, but what the hell ...
     * One day I'll implement one of those fancy algorithms
     * with precalculation.
     */
    switch(p-a) {
    case 3:
	if(eq(a, p, "HDR")) t = HDR;
	break;
    case 4:
	if(eq(a, p, "BODY")) t = BODY;
	else if(eq(a, p, "DATE")) t = DATE;
	else if(eq(a, p, "HEAD")) t = HEAD;
	else if(eq(a, p, "HELP")) t = HELP;
	else if(eq(a, p, "LAST")) t = LAST;
	else if(eq(a, p, "LIST")) t = LIST;
	else if(eq(a, p, "MODE")) t = MODE_READER;
	else if(eq(a, p, "NEXT")) t = NEXT;
	else if(eq(a, p, "OVER")) t = OVER;
	else if(eq(a, p, "POST")) t = POST;
	else if(eq(a, p, "QUIT")) t = QUIT;
	else if(eq(a, p, "STAT")) t = STAT;
	break;
    case 5:
	if(eq(a, p, "GROUP")) t = GROUP;
	else if(eq(a, p, "IHAVE")) t = IHAVE;
	break;
    case 7:
	if(eq(a, p, "ARTICLE")) t = ARTICLE;
	else if(eq(a, p, "NEWNEWS")) t = NEWNEWS;
	break;
    case 9:
	if(eq(a, p, "LISTGROUP")) t = LISTGROUP;
	else if(eq(a, p, "NEWGROUPS")) t = NEWGROUPS;
	break;
    case 12:
	if(eq(a, p, "CAPABILITIES")) t = CAPABILITIES;
	break;
    default:
	break;
    }

    if(t==MODE_READER) {
	while(p!=b && isspace(*p)) p++;
	a = p;
	while(p!=b && !isspace(*p)) p++;
	if(!eq(a, p, "READER")) t = UNKNOWN;
    }

    else if(t==LIST) {
	while(p!=b && isspace(*p)) p++;
	a = p;
	while(p!=b && !isspace(*p)) p++;
	if(eq(a, p, "")) t = LIST;
	else if(eq(a, p, "ACTIVE")) t = LIST_ACTIVE;
	else if(eq(a, p, "ACTIVE.TIMES")) t = LIST_ACTIVE_TIMES;
	else if(eq(a, p, "DISTRIB.PATS")) t = LIST_DISTRIB_PATS;
	else if(eq(a, p, "HEADERS")) t = LIST_HEADERS;
	else if(eq(a, p, "NEWSGROUPS")) t = LIST_NEWSGROUPS;
	else if(eq(a, p, "OVERVIEW.FMT")) t = LIST_OVERVIEW_FMT;
	else t = UNKNOWN;
    }

    return t;
}


Command::Command(ResponseBuf& buf, const std::string& s)
{
    if(s.empty()) {
	buf.ostream() << response::Mode(true, "Hello, world");
    }
    else {
	buf.ostream() << response::Mode(true, "World, hello");
    }
}
