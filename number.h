/* -*- c++ -*-
 * $Id: number.h,v 1.2 2011-03-15 22:03:48 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_NUMBER_H_
#define GB_NUMBER_H_

struct Number {
    explicit Number(unsigned n) : n(n) {}
    const unsigned n;
};

#endif
