/* -*- c++ -*-
 *
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_DEFLATE_H_
#define GB_DEFLATE_H_

#include "blob.h"
#include <vector>

struct z_stream_s;

/**
 * A wrapper around the deflate (compress) part of zlib,
 * specialized for use with Filter::Zlib. You push a number of
 * Blobs, and at any point you may harvest whatever has popped
 * out so far.
 */
class Deflate {
public:
    Deflate();
    ~Deflate();

    void push(const Blob& data);
    void finish();
    Blob front() const { return Blob(out); }
    void pop() { out.clear(); }

private:
    Deflate(const Deflate&);
    Deflate& operator= (const Deflate&);

    z_stream_s* const z;
    std::vector<uint8_t> out;
};

#endif
