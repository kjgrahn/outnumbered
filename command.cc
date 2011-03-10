/* $Id: command.cc,v 1.1 2011-03-10 22:04:04 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "command.h"

#include <cassert>
#include <unistd.h>


Command::Command(int fd, const std::string& s)
    : fd_(fd),
      s_(s)
{}


bool Command::process()
{
    size_t sz = s_.size();
    ssize_t n = write(fd_, s_.data(), sz);
    assert(n>=0 && n==sz);
    return true;
}


bool Command::resume()
{
    assert(0);
    return true;
}
