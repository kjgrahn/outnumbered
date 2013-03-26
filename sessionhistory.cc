/* Copyright (c) 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "session.h"

#include <cassert>


History::History(const timespec& t)
    : t0(t),
      e(0)
{}


/**
 * 'resp' was created at time 't'.  This does not mean that the
 * request was sent at that time, or even received by us -- it's the
 * time we began to process it, and we might have processed many
 * queued-up requests in the meantime.
 */
void History::began(const Response&, const timespec& t)
{
    assert(idle());
    e++;
    tb = t;
}


/**
 * Part (one octet or more) of a response was written.
 */
void History::wrote(const timespec& t)
{
    assert(!idle());
    tw = t;
}


/**
 * 'resp' was concluded.
 */
void History::ended(const Response&, const timespec& t)
{
    assert(!idle());
    e++;
    te = t;
}


/**
 * True if no request is active, and none has been active within the
 * last 's' seconds, assuming the clock is now 't'.
 */
bool History::idle(unsigned s, const timespec& t) const
{
    if(idle()) {
	return t-te > s;
    }
    else {
	return false;
    }
}


/**
 * True if a request is being processed, and nothing has happened for
 * more than 's' seconds.  This is a bit crude; a request for a 1GB
 * document which is written at one octet per second is in practice
 * wedged, too.
 */
bool History::wedged(unsigned s, const timespec& t) const
{
    if(idle()) {
	return false;
    }
    else {
	return t-tb > s;
    }
}
