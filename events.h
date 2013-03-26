/* -*- c++ -*-
 *
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_EVENTS_H_
#define GB_EVENTS_H_

#include <vector>

struct sockaddr_storage;
struct timespec;
struct Session;


/**
 * The background to much of the the epoll loop; the connection
 * between poll events, client sockets and Session objects.
 *
 * This class is the owner of the Sessions, but not the fds.
 *
 * Sessions are stored by pointer in a never-shrinking std::vector,
 * and registered by epoll using their vector index. Thus you can get
 * holes in the vector, NULL Session pointers.
 */
class Events {
public:
    Events();
    ~Events();

    unsigned insert(int fd, sockaddr_storage& peer, const timespec& t);
    void remove(unsigned n);

    int fd(unsigned n) const { return v[n].fd; }
    Session& session(unsigned n) const { return *v[n].session; }

private:
    Events(const Events&);
    Events& operator= (const Events&);

    struct Event {
	Event() : fd(-1), session(0) {}
	Event(int fd, Session* session) : fd(fd), session(session) {}
	bool operator== (const Event& other) const { return fd==other.fd; }
	int fd;
	Session* session;
    };

    std::vector<Event> v;
};

#endif
