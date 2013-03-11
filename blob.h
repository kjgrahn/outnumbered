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
#include <string>


/**
 * Just my usual range-of-octets utility type.
 * Of course, sensitive to changes in the underlying object.
 */
class Blob {
public:
    Blob() : a(0), n(0) {}
    Blob(const char* a, size_t n)
	: a(reinterpret_cast<const uint8_t*>(a)),
	  n(n)
    {}
    Blob(const uint8_t* a, size_t n) : a(a), n(n) {}
    Blob(const uint8_t* a,
	 const uint8_t* b)
	: a(a),
	  n(b-a)
    {}
    Blob(const std::vector<uint8_t>& v)
	: a(&v[0]),
	  n(v.size())
    {}
    Blob(const std::string& s)
	: a(reinterpret_cast<const uint8_t*>(s.c_str())),
	  n(s.size())
    {}

    const uint8_t* begin() const { return a; }
    const uint8_t* end() const { return a + n; }

    bool empty() const { return !n; }
    size_t size() const { return n; }
    operator const void*() const { return empty()? 0: this; }

private:
    const uint8_t* a;
    const size_t n;
};

#endif
