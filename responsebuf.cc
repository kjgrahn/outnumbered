/* $Id: responsebuf.cc,v 1.2 2011-03-15 22:03:48 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "responsebuf.h"

#include "response.h"


ResponseBuf::ResponseBuf(int fd)
    : fd_(fd)
{}


bool ResponseBuf::empty() const
{
    return true;
}


std::string ResponseBuf::str() const
{
    return oss_.str();
}


template<class Resp>
void ResponseBuf::write(const Resp& resp)
{
    oss_ << resp;
}

template void ResponseBuf::write(const response::Next& resp);
template void ResponseBuf::write(const response::Quit& resp);
template void ResponseBuf::write(const response::Date& resp);
template void ResponseBuf::write(const response::Mode& resp);
template void ResponseBuf::write(const response::Group& resp);
