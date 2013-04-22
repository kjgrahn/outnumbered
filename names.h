/* -*- c++ -*-
 *
 * Copyright (c) 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OUTND_NAMES_H
#define OUTND_NAMES_H

#include "request.h"

#include <tr1/unordered_map>
#include <string>

/**
 * Lookup string -> Request::Property for:
 * - request methods
 * - HTTP versions
 * - header names
 *
 * This doesn't cover all possible properties, e.g. it doesn't make
 * sense to look up UNKNOWN by name.
 */
class Names {
public:
    Names();
    Request::Property lookup(const char* a, const char* b) const;

private:
    typedef std::tr1::unordered_map<std::string, Request::Property> Map;
    Map map;
};

#endif

