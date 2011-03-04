/* $Id: gresabladet.cc,v 1.9 2011-03-04 22:55:12 grahn Exp $
 *
 * Copyright (c) 2010, 2011 J�rgen Grahn
 * All rights reserved.
 *
 */
#include <iostream>
#include <vector>
#include <algorithm>

#include <getopt.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

#include "version.h"
#include "session.h"

namespace {

    bool reuse_addr(int fd)
    {
	int val = 1;
	return setsockopt(fd,
			  SOL_SOCKET, SO_REUSEADDR,
			  &val, sizeof val) == 0;
    }

    bool nonblock(int fd)
    {
	int n = fcntl(fd, F_GETFL, 0);
	if(n<0) return false;
	return fcntl(fd, F_SETFL, n | O_NONBLOCK) >= 0;
    }

    void ignore_sigpipe()
    {
	static struct sigaction ignore;
	ignore.sa_handler = SIG_IGN;
	(void)sigaction(SIGPIPE, &ignore, 0);
    }

    /* Create a listening socket on host:port (the wildcard address if
     * host is empty). Does everything including listen(), and prints
     * relevant error messages on std::error.
     */
    int listening_socket(const std::string& host,
			 const std::string& port)
    {
	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	struct addrinfo *result;
	const int s = getaddrinfo(host.empty()? 0: host.c_str(),
				  port.c_str(),
				  &hints, &result);
	if(s) {
	    std::cerr << "error: " << gai_strerror(s) << '\n';
	    return -1;
	}

	int fd;
	const addrinfo* rp;
	for(rp = result; rp; rp = rp->ai_next) {
	    const addrinfo& r = *rp;

	    fd = socket(r.ai_family, r.ai_socktype, r.ai_protocol);
	    if(fd == -1) continue;

	    if(reuse_addr(fd) && bind(fd, r.ai_addr, r.ai_addrlen) == 0) {
		break;
	    }

	    close(fd);
	}

	freeaddrinfo(result);

	if(!rp || listen(fd, 10)==-1) {
	    std::cerr << "socket error: " << strerror(errno) << '\n';
	    return -1;
	}

	return fd;
    }


    /* Sessions are stored by pointer in a never-shrinking std::vector, and registered
     * by epoll using their vector index. Thus you can get holes in the vector, NULL
     * Session pointers.
     */

    typedef std::vector<Session*> Sessions;
    static Session* const nullsession = 0;

    unsigned insert_session(Sessions& sessions, int fd, struct sockaddr_storage& sa)
    {
	Sessions::iterator i = std::find(sessions.begin(), sessions.end(), nullsession);
	if(i==sessions.end()) {
	    sessions.push_back(0);
	    i = sessions.end() - 1;
	}
	*i = new Session(fd, sa);
	return i - sessions.begin();
    }


    void remove_session(Sessions& sessions, unsigned n)
    {
	delete sessions[n];
	sessions[n] = nullsession;
    }


    /**
     * The main event loop. It is epoll(7)-based not only because it's
     * more scalable than select(2), but because it's a much better
     * interface, with the event structs being managed by the kernel
     * and all. But yes, it's Linux-specific.
     */
    bool loop(const int epfd, const int lfd)
    {
	static const unsigned LFD = ~0;

	{
	    epoll_event ev = {EPOLLIN, {0}};
	    ev.data.u32 = LFD;
	    if(epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev)==-1) {
		return false;
	    }
	}

	Sessions sessions;

	while(1) {
	    epoll_event events[10];
	    const int n = epoll_wait(epfd, events, 10, -1);
	    for(int i=0; i<n; ++i) {
		const epoll_event& ev = events[i];

		if(ev.data.u32==LFD) {
		    struct sockaddr_storage sa;
		    socklen_t slen = sizeof sa;
		    int fd = accept(lfd, reinterpret_cast<sockaddr*>(&sa), &slen);
		    if(fd!=-1) {
			nonblock(fd);

			const unsigned n = insert_session(sessions, fd, sa);
			epoll_event ev = {EPOLLIN, {0}};
			// ev.events = EPOLLIN | EPOLLOUT;
			ev.data.u32 = n;
			if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev)==-1) {
			    remove_session(sessions, n);
			}
		    }
		    continue;
		}

		Session& session = *sessions[ev.data.u32];
		session.feed();
		if(session.eof()) {
		    epoll_ctl(epfd, EPOLL_CTL_DEL, session.fd(), 0);
		    remove_session(sessions, ev.data.u32);
		}
	    }
	}

	return true;
    }

}


int main(int argc, char ** argv)
{
    using std::string;

    const string prog = argv[0];
    const string usage = string("usage: ")
	+ prog +
	" [-d]"
	" [-a listen-address]"
	" [-p port]";
    const char optstring[] = "+dp:a:";
    struct option long_options[] = {
	{"--daemon",       0, 0, 'd'},
	{"--address",      1, 0, 'a'},
	{"--port",         1, 0, 'p'},
	{"version", 	   0, 0, 'v'},
	{"help",    	   0, 0, 'h'},
	{0, 0, 0, 0}
    };

    bool daemon = false;
    string addr = "";
    string port = "nntp";

    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);

    int ch;
    while((ch = getopt_long(argc, argv,
			    optstring, &long_options[0], 0)) != -1) {
	switch(ch) {
	case 'd':
	    daemon = true;
	    break;
	case 'a':
	    addr = optarg;
	    break;
	case 'p':
	    port = optarg;
	    break;
	case 'h':
	    std::cout << usage << '\n';
	    return 0;
	case 'v':
	    std::cout << "gresabladet " << version() << '\n'
		      << "Copyright (c) 2010 J�rgen Grahn\n";
	    return 0;
	    break;
	case ':':
	case '?':
	    std::cerr << usage << '\n';
	    return 1;
	    break;
	default:
	    break;
	}
    }

    const int lfd = listening_socket(addr, port);
    if(lfd==-1) {
	return 1;
    }

    ignore_sigpipe();

    const int epfd = epoll_create(10);
    if(epfd==-1) {
	std::cerr << "error: failed to create epoll fd: " << strerror(errno) << '\n';
	return 1;
    }

    loop(epfd, lfd);

    close(epfd);
    close(lfd);

    return 0;
}
