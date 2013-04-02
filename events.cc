/*
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "events.h"
#include "session.h"

#include <algorithm>
#include <netinet/in.h>


Events::Events()
{
}


Events::~Events()
{
    for(iterator i = v.begin(); i != v.end(); i++) {
	delete i->session;
    }
}


/**
 * Connect 'fd' with a new Session, and assign an index suitable for
 * epoll to the result.
 */
unsigned Events::insert(int fd, struct sockaddr_storage& sa,
			const timespec& t)
{
    const Event nullevent;
    iterator i = std::find(v.begin(), v.end(), nullevent);
    if(i==v.end()) {
	v.push_back(Event(fd, new Session(sa, t)));
	i = v.end() - 1;
    }
    else {
	*i = Event(fd, new Session(sa, t));
    }
    return i - v.begin();
}


void Events::remove(unsigned n)
{
    const Event nullevent;
    delete v[n].session;
    v[n] = nullevent;
}
