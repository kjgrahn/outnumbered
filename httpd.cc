/*
 * Copyright (c) 2010--2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

#include <getopt.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include "version.h"
#include "error.h"
#include "server.h"
#include "session.h"


namespace {

    bool reuse_addr(int fd)
    {
	int val = 1;
	return setsockopt(fd,
			  SOL_SOCKET, SO_REUSEADDR,
			  &val, sizeof val) == 0;
    }

    bool setbuf(int fd, int rx, int tx)
    {
	int err;
	err = setsockopt(fd, SOL_SOCKET, SO_SNDBUF,
			 &tx, sizeof tx);
	if(err) return false;
	err = setsockopt(fd, SOL_SOCKET, SO_RCVBUF,
			 &rx, sizeof rx);
	return !err;
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
     * relevant error messages on stderr.
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

	    if(reuse_addr(fd)
	       && bind(fd, r.ai_addr, r.ai_addrlen) == 0) {
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


    /**
     * The main event loop.
     */
    bool loop(const int lfd)
    {
	Server server;
	server.add(lfd);
	timespec ts = now();
	Periodic audit(ts, 20);

	while(1) {
	    server.wait(20*1000);
	    ts = now();

	    for(Server::iterator i = server.lbegin(); i!=server.lend(); i++) {
		Server::Event& ev = *i;
		Server::Entry& entry = server.entry(ev);

		struct sockaddr_storage sa;
		socklen_t slen = sizeof sa;
		int fd = accept(entry.fd,
				reinterpret_cast<sockaddr*>(&sa), &slen);
		if(fd!=-1) {
		    nonblock(fd);
		    server.add(fd, sa, ts);
		}
	    }

	    for(Server::iterator i = server.begin(); i!=server.end(); i++) {
		Server::Event& ev = *i;
		Server::Entry& entry = server.entry(ev);

		Session& session = *entry.session;
		const int fd = entry.fd;
		Session::State state = Session::DIE;

		try {
		    if(ev.writable()) {
			state = session.write(fd, ts);
		    }
		    else if(ev.readable()) {
			state = session.read(fd, ts);
		    }
		}
		catch(const SessionError&) {
		    ;
		}

		if(state==Session::DIE) {
		    server.remove(ev);
		}
		else {
		    server.ctl(ev, state);
		}
	    }

	    if(audit.check(ts)) {
		server.reconsider(ts);
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

    bool daemonize = false;
    string addr = "";
    string port = "http";

    int ch;
    while((ch = getopt_long(argc, argv,
			    optstring, &long_options[0], 0)) != -1) {
	switch(ch) {
	case 'd':
	    daemonize = true;
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
	    std::cout << "outnumbered " << version() << '\n'
		      << "Copyright (c) 2010-2013 Jörgen Grahn\n";
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

    if(daemonize) {
	int err = daemon(0, 0);
	if(err) {
	    std::cerr << "error: failed to move to the background: "
		      << strerror(errno) << '\n';
	    return 1;
	}
    }

    loop(lfd);

    return 0;
}
