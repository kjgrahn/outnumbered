/* -*- c++ -*-
 *
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_BLOB_H_
#define GB_BLOB_H_

#include <stdint.h>


/**
 * Just my usual range-of-octets utility type.
 */
class Blob {
public:
    Blob() : a(0), b(0) {}
    Blob(const uint8_t* a,
	 const uint8_t* b)
	: a(a), b(b)
    {}
    bool empty() const { return a==b; }
    operator const void*() const { return empty()? 0: a; }
    const uint8_t* a;
    const uint8_t* b;
};

#endif
