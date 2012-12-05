/* $Id: responsebuf.cc,v 1.14 2011-07-03 08:27:21 grahn Exp $
 *
 * Copyright (c) 2012 Jörgen Grahn
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
