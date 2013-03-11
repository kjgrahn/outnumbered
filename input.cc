/*
 * Copyright (c) 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "input.h"

#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


InputFd::InputFd(const std::string& path)
    : fd(open(path.c_str(), O_RDONLY)),
      eof(false),
      err(fd>=0? 0: errno)
{}


InputFd::~InputFd()
{
    if(fd>=0) close(fd);
}


/**
 * Like read(2), but returns 0 on errors too; sets eof and err (errno)
 * instead.
 */
size_t InputFd::read(void *buf, size_t count)
{
    if(!alive()) return 0;
    ssize_t n = ::read(fd, buf, count);
    switch(n) {
    case -1:
	err = errno;
	n = 0;
	break;
    case 0:
	eof = true;
	break;
    }

    return n;
}
