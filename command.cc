/* $Id: command.cc,v 1.3 2011-03-28 22:04:38 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "command.h"
#include "responsebuf.h"
#include "response.h"

#include <cassert>
#include <unistd.h>


Command::Command(ResponseBuf& buf, const std::string& s)
{
    if(s.empty()) {
	buf.write(response::Mode(true, "Hello, world"));
    }
    else {
	buf.write(response::Mode(true, "World, hello"));
    }
}
