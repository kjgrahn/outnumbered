/* -*- c++ -*-
 * $Id: command.h,v 1.1 2011-03-10 22:04:05 grahn Exp $
 *
 * Copyright (c) 2010, 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_COMMAND_H_
#define GB_COMMAND_H_

#include <string>

class Command {
public:
    Command(int fd, const std::string& s);
    bool process();
    bool resume();

private:
    Command(const Command&);
    Command& operator= (const Command&);
    int fd_;
    std::string s_;
};

#endif
