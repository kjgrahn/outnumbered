/* -*- c++ -*-
 * $Id: msgid.h,v 1.2 2011-03-15 22:03:48 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_MSGID_H_
#define GB_MSGID_H_

struct MsgId {
    explicit MsgId(const char* val) : val(val) {}
    const char* const val;
};

#endif
