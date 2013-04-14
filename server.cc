/*
 * Copyright (c) 2012, 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "server.h"
#include "session.h"
#include "error.h"

#include <algorithm>
#include <cassert>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/epoll.h>

namespace {

    static const unsigned LFD = ~0;

    unsigned insert(std::vector<Server::Entry>& v)
    {
	unsigned i;
	for(i=0; i<v.size(); i++) {
	    if(v[i].empty()) break;
	}

	if(i<v.size()) return i;
	v.push_back(Server::Entry());
	return i;
    }

    /**
     * Insert a listening entry, returning its index.
     * Note that this may invalidate iterators into the vector,
     * and also resurrect a recently used index.
     */
    unsigned insert(std::vector<Server::Entry>& v, int lfd)
    {
	unsigned n = insert(v);
	Server::Entry& entry = v[n];
	entry = Server::Entry(lfd);
	return n;
    }

    /**
     * Insert a session entry, returning its index.
     */
    unsigned insert(std::vector<Server::Entry>& v, int fd, Session* session)
    {
	unsigned n = insert(v);
	Server::Entry& entry = v[n];
	entry = Server::Entry(fd, session);
	return n;
    }

    /**
     * Remove an entry, freeing the resources it owns and making the
     * entry itself available for reuse.
     *
     * (Incidentally, closing an fd also removes it from the epoll
     * set.)
     */
    void remove(std::vector<Server::Entry>::iterator i)
    {
	Server::Entry& entry = *i;
	if(!entry.empty()) {
	    close(entry.fd);
	    delete entry.session;
	}
	entry = Server::Entry();
    }

    void remove(std::vector<Server::Entry>& v, unsigned n)
    {
	remove(v.begin() + n);
    }

    epoll_event mkev(unsigned events, unsigned n)
    {
	epoll_event ev;
	ev.events = events;
	ev.data.u64 = 0; /* to please strace(1) */
	ev.data.u32 = n;
	return ev;
    }
}


Server::Server()
    : epfd(epoll_create(10))
{
    if(epfd==-1) throw FatalError();
}


Server::~Server()
{
    for(std::vector<Entry>::iterator i = v.begin(); i!=v.end(); i++) {
	if(!i->empty()) {
	    close(i->fd);
	    delete i->session;
	}
    }
    close(epfd);
}


Server::Entry::Entry()
    : fd(-1),
      session(0)
{}


Server::Entry::Entry(int lfd)
    : fd(lfd),
      session(0)
{}


Server::Entry::Entry(int fd, Session* session)
    : fd(fd),
      session(session)
{}


/**
 * Add a listening socket, monitored for readability.
 */
void Server::add(int lfd)
{
    unsigned n = insert(v, lfd);
    epoll_event ep = mkev(EPOLLIN, n);
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ep)==-1) {
	::remove(v, n);
    }
}


/**
 * Add a session (client) socket and its associated Session. It starts
 * out monitored for readability.
 */
void Server::add(int fd, sockaddr_storage& peer, const timespec& t)
{
    unsigned n = insert(v, fd, new Session(peer, t));
    epoll_event ep = mkev(EPOLLIN, n);
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ep)==-1) {
	::remove(v, n);
    }
}


/**
 * Delete the Session associated with 'ev', along with its fd.
 */
void Server::remove(const Event& ev)
{
    ::remove(v, ev.n);
}


/**
 * Perform EPOLL_CTL_MOD(state) for the epoll descriptor associated
 * with 'ev'.
 */
void Server::ctl(Event ev, unsigned state)
{
    Entry& entry = v[ev.n];
    if(state != entry.events) {
	entry.events = state;
	epoll_event ep = mkev(state, ev.n);
	if(epoll_ctl(epfd, EPOLL_CTL_MOD, entry.fd, &ep)==-1) {
	    ::remove(v, ev.n);
	}
    }
}


/**
 * Like epoll_wait(..., timeout), except events become available in
 * [lbegin..lend) and [begin, end) (listening/session fds,
 * respectively).
 */
int Server::wait(int timeout)
{
    nend = nlend = 0;
    const int max = sizeof ev / sizeof ev[0];
    epoll_event ep[max];
    const int n = epoll_wait(epfd, ep, max, timeout);
    if(n<1) return n;

    unsigned listeners = 0;
    Event* p = ev;
    for(int i=0; i<n; i++) {
	const Entry& entry = v[ep[i].data.u32];
	if(entry.client()) {
	    p->events = ep[i].events;
	    p->n = ep[i].data.u32;
	    p++;
	}
	else if(entry.listening()) {
	    listeners++;
	}
    }
    nend = (p - ev);
    nlend = nend + listeners;

    if(listeners) {
	for(int i=0; i<n; i++) {
	    const Entry& entry = v[ep[i].data.u32];
	    if(entry.listening()) {
		p->events = ep[i].events;
		p->n = ep[i].data.u32;
		p++;
	    }
	}
    }

    return n;
}


/**
 * Let all Sessions reconsider(), and kill the ones which feel a need
 * to die.
 */
void Server::reconsider(const timespec& ts)
{
    for(std::vector<Entry>::iterator i = v.begin(); i!=v.end(); i++) {
	if(i->client() && i->session->reconsider(ts)) {
	    ::remove(i);
	}
    }
}
