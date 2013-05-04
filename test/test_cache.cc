/*
 * Copyright (C) 2013 Jörgen Grahn.
 * All rights reserved.
 */
#include <headercache.h>

#include <testicle.h>
#include <string>
#include <sstream>


namespace {

    /**
     * strtol(3) on a range, and without sane error checking.
     */
    long tol(const char* a, const char* b)
    {
	std::string s(a, b-a);
	return std::strtol(s.c_str(), 0, 0);
    }

    struct Integer {
	Integer() : val(0) {}
	Integer(const char* a, const char* b)
	    : val(tol(a, b))
	{}
	int val;
    };

    typedef HeaderCache<Integer> Cache;
}


namespace cache {

    using testicle::assert_eq;

    void test_construct()
    {
	Cache c;
    }

    void test_simple()
    {
	Cache c;
	assert_eq(c.insert("42").val, 42);
    }

    void test_overflow()
    {
	Cache c;
	for(int i=0; i<200; i++) {
	    std::ostringstream oss;
	    oss << i;
	    assert_eq(c.insert(oss.str()).val, i);
	    assert_eq(c.insert(oss.str()).val, i);
	    assert_eq(c.insert(oss.str()).val, i);
	}
    }
}
