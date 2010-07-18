/* $Id: gresabladet.cc,v 1.1 2010-07-18 18:59:01 grahn Exp $
 *
 * Copyright (c) 2010 Jörgen Grahn
 * All rights reserved.
 *
 */
#include <iostream>
#include <fstream>
#include <vector>

#include <cstdlib>
#include <cerrno>
#include <getopt.h>

#include "bibdb.h"
#include "twinstate.h"
#include "filter.h"
#include "cite.h"
#include "version.h"


int main(int argc, char ** argv)
{
    using std::string;

    const string prog = argv[0];
    const string usage = string("usage: ")
	+ prog +
	" [-d]"
	" [-a listen-address]"
	" [-p port]"
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
		      << "Copyright (c) 2010 Jörgen Grahn\n";
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

    return 0;
}
