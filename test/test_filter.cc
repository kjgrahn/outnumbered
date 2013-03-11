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

    using testicle::assert_eq;

    namespace plain {

	void test_simple()
	{
	    Pipe p;
	    Filter::P f;
	    assert_eq(write(f, p, "foo"), true);
	    p.assert_read("foo");
	    p.assert_empty();
	}
    }
}
