/*
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "deflate.h"
#include "error.h"

#include <zlib.h>

#include <cassert>

/* The zlib deflate interface is a bit weirdly documented,
 * but it works like this: deflate(stream, Z_NO_FLUSH)
 *
 * - consumes all or part of the input buffer
 * - changes internal stream state
 * - fills none, part or all of the output buffer
 *
 * The zpipe.c example simply reads a file in 16K blocks.  Each block
 * is prepared as an input buffer. Then deflate() is called
 * repeatedly. When it results in a non-full output buffer, that also
 * means the input buffer has been consumed and we can switch to
 * another one and continue.
 *
 */

Deflate::Deflate()
    : z(new(z_stream_s))
{
    z->zalloc = 0;
    z->zfree = 0;
    z->opaque = 0;
    int ret = deflateInit(z, Z_DEFAULT_COMPRESSION);
    assert(ret==Z_OK);
}


Deflate::~Deflate()
{
    (void)deflateEnd(z);
    delete z;
}


/**
 * Feed 'data' into the deflater, and if anything pops out append it to
 * our output.
 */
void Deflate::push(const Blob& data)
{
    uint8_t buf[1000];
    z->next_in = const_cast<uint8_t*>(data.a);
    z->avail_in = data.size();

    while(z->avail_in) {
	z->next_out = buf;
	z->avail_out = sizeof buf;

	int rc = deflate(z, Z_NO_FLUSH);
	if(rc<0) throw WriteError();

	out.insert(out.end(),
		   buf,
		   buf + sizeof buf - z->avail_out);
    }
}


/**
 * Final flushing of the deflater's state to our output.
 */
void Deflate::finish()
{
    uint8_t buf[1000];
    z->next_in = 0;
    z->avail_in = 0;

    do {
	z->next_out = buf;
	z->avail_out = sizeof buf;

	int rc = deflate(z, Z_FINISH);
	if(rc<0) throw WriteError();

	out.insert(out.end(),
		   buf,
		   buf + sizeof buf - z->avail_out);
    } while(!z->avail_out);
}
