/*
 * Copyright (c) 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#include <iostream>
#include <string>

#include <magic.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "version.h"


class Magic {
public:
    explicit Magic(int flags);
    ~Magic();

    const char* mime(const void* buf, size_t len) const;

private:
    Magic(const Magic&);
    Magic& operator= (const Magic&);
    magic_t cookie;
};


Magic::Magic(int flags)
    : cookie(magic_open(flags |
			MAGIC_MIME |
			MAGIC_ERROR))
{
    (void)magic_load(cookie, NULL);
}


Magic::~Magic()
{
    magic_close(cookie);
}


const char* Magic::mime(const void* buf, size_t len) const
{
    return magic_buffer(cookie, buf, len);
}


const char* check(const Magic& magic, const char* path)
{
    const char* s = "";
    int fd = open(path, O_RDONLY);
    if(fd>=0) {
	char buf[4096];
	ssize_t n = read(fd, buf, sizeof buf);
	if(n>=0) {
	    s = magic.mime(buf, n);
	}
	close(fd);
    }
    return s;
}


int main(int argc, char ** argv)
{
    using std::string;

    const string prog = argv[0];
    const string usage = string("usage: ")
	+ prog +
	" [--debug]"
	" [--compress]"
	" file ...";
    const char optstring[] = "+dz";
    struct option long_options[] = {
	{"--debug",        0, 0, 'd'},
	{"--compress",     1, 0, 'z'},
	{"version", 	   0, 0, 'v'},
	{"help",    	   0, 0, 'h'},
	{0, 0, 0, 0}
    };

    int flags = 0;

    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);

    int ch;
    while((ch = getopt_long(argc, argv,
			    optstring, &long_options[0], 0)) != -1) {
	switch(ch) {
	case 'd':
	    flags |= MAGIC_DEBUG;
	    break;
	case 'z':
	    flags |= MAGIC_COMPRESS;
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

    const char* const * name = &argv[optind];
    const char* const * const end = &argv[argc];
    Magic magic(flags);

    while(name != end) {
	std::cout << check(magic, *name) << '\n';
	name++;
    }

    return 0;
}
