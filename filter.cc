/*
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "filter.h"

#include <errno.h>
#include <sys/uio.h>


using Filter::Plain;
using Filter::Chunked;
using Filter::Zlib;


namespace {
    int write(int fd, const Blob& a);
    int write(int fd, const Blob& a, const Blob& b);
}


bool Plain::write(int fd)
{
    if(backlog.empty()) return true;

    Blob a = backlog.buf();
    int n = write(fd, a);

}
