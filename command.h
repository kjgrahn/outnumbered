/* -*- c++ -*-
 * $Id: command.h,v 1.2 2011-03-12 23:51:55 grahn Exp $
 *
 * Copyright (c) 2010, 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_COMMAND_H_
#define GB_COMMAND_H_

#include <string>

class ResponseBuf;

class Command {
public:
    Command(ResponseBuf& buf, const std::string& s);

private:
    Command(const Command&);
    Command& operator= (const Command&);
};

#endif
