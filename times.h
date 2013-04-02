/* -*- c++ -*-
 *
 * Copyright (c) 2012, 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_TIME_H_
#define GB_TIME_H_

#include <time.h>

timespec now();
timespec operator- (const timespec& a, const timespec& b);
bool operator> (const timespec& a, unsigned b);


/**
 * Simple utility for keeping track of periodic work.
 *   +---+---+---+---+---+---+---+---+···
 *   t0
 * You simply ask it "have I passed a deadline since last time I
 * asked?"  Additionally, it can suggest a epoll_wait() timeout.
 */
class Periodic {
public:
    Periodic(const timespec& t, unsigned dt);
    bool check(const timespec& t);
    int timeout(const timespec& t) const;
};

#endif
