/* -*- c++ -*-
 *
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_DEFLATE_H_
#define GB_DEFLATE_H_

#include "blob.h"

/**
 * A wrapper around the deflate (compress) part of zlib.
 */
class Deflate {
public:
    Deflate();
    ~Deflate();

    void push(const Blob& data);
    void flush();
    Blob pop();

private:
    Deflate(const Deflate&);
    Deflate& operator= (const Deflate&);
};

#endif
