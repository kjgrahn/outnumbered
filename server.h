/* -*- c++ -*-
 *
 * Copyright (c) 2012, 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_SERVER_H_
#define GB_SERVER_H_

#include <vector>
#include <sys/epoll.h>

struct sockaddr_storage;
struct timespec;
struct Session;


/**
 * The background to much of the the epoll loop; the connection
 * between poll events, client sockets and Session objects.
 * Part of the core of an epoll-based TCP server.
 *
 * This class is the owner of Sessions and fds (listening sockets
 * and connected ones).
 *
 */
class Server {
public:
    Server();
    ~Server();

    struct Entry {
	unsigned events;
	int fd;
	Session* session;

	Entry();
	explicit Entry(int lfd);
	Entry(int fd, Session* session);
	bool listening() const { return !session && fd!=-1; }
	bool client() const { return session; }
	bool empty() const { return fd==-1; }
    };

    struct Event {
	unsigned n;
	unsigned events;
	bool writable() const { return events & EPOLLOUT; }
	bool readable() const { return events & EPOLLIN; }
    };
    typedef Event* iterator;

    Entry& entry(const Event& event) { return v[event.n]; }

    void add(int lfd);
    void add(int fd, sockaddr_storage& peer, const timespec& t);
    void remove(const Event& ev);
    void ctl(Event ev, unsigned state);

    int wait(int timeout);
    iterator begin()  { return ev; }
    iterator end()    { return ev+nend; }
    iterator lbegin() { return end(); }
    iterator lend()   { return ev+nlend; }

    void reconsider(const timespec& ts);

private:
    Server(const Server&);
    Server& operator= (const Server&);

    std::vector<Entry> v;
    const int epfd;
    unsigned short nlend;
    unsigned short nend;
    Event ev[20];
};

#endif
