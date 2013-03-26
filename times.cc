/* $Id: responsebuf.cc,v 1.14 2011-07-03 08:27:21 grahn Exp $
 *
 * Copyright (c) 2012, 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "times.h"


/**
 * Shorthand for clock_gettime(CLOCK_REALTIME).
 */
timespec now()
{
    timespec ts;
    (void)clock_gettime(CLOCK_REALTIME, &ts);
    return ts;
}


/**
 * t - t0.  Undefined unless 't' is 't0' or a later time.
 * Undefined if 't' or 't0' is not normalized.
 */
timespec operator- (const timespec& t, const timespec& t0)
{
    timespec n = t;
    n.tv_sec -= t0.tv_sec;
    if(n.tv_nsec >= t0.tv_nsec) {
	n.tv_nsec -= t0.tv_nsec;
    }
    else {
	n.tv_nsec = 1000*1000*1000 - (t0.tv_nsec - n.tv_nsec);
	n.tv_sec--;
    }
    return n;
}


/**
 * True if a delta timespec 'a' is greater than 'b' seconds.
 */
bool operator> (const timespec& a, unsigned b)
{
    return a.tv_sec > b || (a.tv_sec == b && a.tv_nsec);
}
