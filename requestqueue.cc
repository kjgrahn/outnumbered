/*
 * Copyright (c) 2012, 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "requestqueue.h"


void RequestQueue::add(const char* a, const char* b)
{
    if(val.empty() || val.back().complete) {
	val.push(Request());
    }
    val.back().add(a, b);
}


/**
 * True if there's a complete Request.
 */
bool RequestQueue::complete() const
{
    if(val.empty()) return false;
    return val.front().complete;
}

/**
 * True if there's a complete Request, and it's broken.
 *
 * This generally means something worse than just an unsupported
 * request method or something; it makes the whole session suspect.
 */
bool RequestQueue::broken() const
{
    if(val.empty()) return false;
    return val.front().broken;
}


const Request& RequestQueue::front() const
{
    return val.front();
}


void RequestQueue::pop()
{
    val.pop();
}
