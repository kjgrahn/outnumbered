/* -*- c++ -*-
 * $Id: number.h,v 1.3 2011-03-16 21:23:19 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_NUMBER_H_
#define GB_NUMBER_H_

struct Number {
    explicit Number(unsigned n) : val(n) {}
    const unsigned val;
};

#endif
