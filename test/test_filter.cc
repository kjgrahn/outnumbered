/*
 * Copyright (C) 2013 Jörgen Grahn.
 * All rights reserved.
 */
#include <filter.h>
#include "pipe.h"

#include <testicle.h>

namespace {
    template<class Filter>
    size_t write(Filter& filter, Pipe& pipe,
		 const std::string& s)
    {
	return filter.write(pipe.fd(), Blob(s));
    }

    template<class Filter>
    size_t write(Filter& filter, Pipe& pipe,
		 const std::string& s,
		 const std::string& t)
    {
	return filter.write(pipe.fd(), Blob(s), Blob(t));
    }

    template<class Filter>
    size_t write(Filter& filter, Pipe& pipe,
		 const std::string& s,
		 const std::string& t,
		 const std::string& u)
    {
	return filter.write(pipe.fd(), Blob(s), Blob(t), Blob(u));
    }
}


namespace filter {

    static const Blob karen("I do understand what you're saying;\n"
			    "it just doesn't make any sense!\n");

    using testicle::assert_eq;
    using testicle::assert_gt;

    namespace plain {

	void test_simple()
	{
	    Pipe p;
	    Filter::P f;
	    assert_eq(write(f, p, "foo"), true);
	    p.assert_read("foo");
	    p.assert_empty();
	    assert_eq(write(f, p, "fo", "o"), true);
	    p.assert_read("foo");
	    p.assert_empty();
	    assert_eq(write(f, p, "f", "o", "o"), true);
	    p.assert_read("foo");
	    p.assert_empty();
	}

	void test_block()
	{
	    Pipe p;
	    Filter::P f;

	    size_t n = 0;
	    while(f.write(p.fd(), karen, karen)) {
		n += 2*karen.size();
	    }
	    assert_gt(n, 0);

	    while(n) {
		p.assert_read(karen);
		n -= karen.size();
	    }
	    p.assert_empty();

	    /* now a new write will pass through, along with
	     * the failed one above
	     */
	    assert_eq(f.write(p.fd(), karen), true);
	    p.assert_read(karen);
	    p.assert_read(karen);
	    p.assert_read(karen);
	    p.assert_empty();
	}
    }
}
