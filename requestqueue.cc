/*
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "requestqueue.h"


RequestQueue::RequestQueue()
{}


void RequestQueue::push(const char* a, const char* b)
{
    buf.push_back(std::string(a, b));
}


bool RequestQueue::bad() const
{
    return false;
}


bool RequestQueue::complete() const
{
    return !buf.empty();
}


const std::string& RequestQueue::front() const
{
    return buf.front();
}


void RequestQueue::pop()
{
    buf.pop_front();
}
