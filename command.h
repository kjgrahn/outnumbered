/* -*- c++ -*-
 * $Id: command.h,v 1.3 2011-03-28 22:04:38 grahn Exp $
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
    explicit Command(ResponseBuf& buf, const std::string& s);

private:
    Command(const Command&);
    Command& operator= (const Command&);
};

#endif
