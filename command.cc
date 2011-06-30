/* $Id: command.cc,v 1.4 2011-06-30 09:15:29 grahn Exp $
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
	buf.ostream() << response::Mode(true, "Hello, world");
    }
    else {
	buf.ostream() << response::Mode(true, "World, hello");
    }
}
