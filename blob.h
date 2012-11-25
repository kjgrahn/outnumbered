/* -*- c++ -*-
 *
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_BLOB_H_
#define GB_BLOB_H_

#include <stdint.h>
#include <cstdlib>
#include <vector>


/**
 * Just my usual range-of-octets utility type.
 */
class Blob {
public:
    Blob() : a(0), b(0) {}
    Blob(const char* a, size_t n);
    Blob(const uint8_t* a, size_t n);
    Blob(const uint8_t* a,
	 const uint8_t* b)
	: a(a), b(b)
    {}
    Blob(const std::vector<uint8_t>&);
    bool empty() const { return a==b; }
    size_t size() const;
    operator const void*() const { return empty()? 0: a; }
    const uint8_t* a;
    const uint8_t* b;
};

#endif
