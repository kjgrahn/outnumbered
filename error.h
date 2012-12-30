/* -*- c++ -*-
 *
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_ERROR_H_
#define GB_ERROR_H_

struct WriteError {
    explicit WriteError(int err = 0) : err(err) {}
    int err;
};

#endif
