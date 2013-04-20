/*
 * Copyright (C) 2012, 2013 Jörgen Grahn.
 * All rights reserved.
 */
#include <request.h>

#include <testicle.h>
#include <string>


namespace {

    void add(Request& req, const std::string& s)
    {
	const char* p = s.c_str();
	req.add(p, p+s.size());
    }
}


namespace req {

    using testicle::assert_;
    using testicle::assert_eq;

    void test_simple()
    {
	Request req;
	add(req, "GET /pub/WWW/TheProject.html HTTP/1.1");
	add(req, "Host: www.w3.org");
	add(req, "");

	assert_(req.complete);
	assert_(!req.broken);
	assert_eq(req.method, Request::GET);
	assert_eq(req.request_uri(), "/pub/WWW/TheProject.html");
	assert_eq(req.version, Request::HTTP11);
    }

    void test_opera()
    {
	Request req;
	add(req, "GET /contentfile/imagecrop/1.7867487?cropid=f169w225 HTTP/1.1");
	add(req, "User-Agent: Opera/9.80 (X11; Linux x86_64) Presto/2.12.388 Version/12.11");
	add(req, "Host: www.nrk.no");
	add(req, "Accept: text/html, application/xml;q=0.9, application/xhtml+xml, "
		 "image/png, image/webp, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
	add(req, "Accept-Language: en,sv;q=0.9");
	add(req, "Accept-Encoding: gzip, deflate");
	add(req, "Cache-Control: no-cache");
	add(req, "Connection: Keep-Alive");
	add(req, "");

	assert_(req.complete);
	assert_(!req.broken);
    }

    namespace reqline {

	void test_simple()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    add(req, "");
	    assert_(req.complete);
	    assert_(!req.broken);
	    assert_eq(req.method, Request::GET);
	    assert_eq(req.request_uri(), "foo");
	    assert_eq(req.version, Request::HTTP11);
	}

	void test_case()
	{
	    Request req;
	    /* extension; the method is really case-sensitive [5.1.1] */
	    add(req, "get foo Http/1.1");
	    add(req, "");
	    assert_(req.complete);
	    assert_(!req.broken);
	    assert_eq(req.method, Request::GET);
	    assert_eq(req.request_uri(), "foo");
	    assert_eq(req.version, Request::HTTP11);
	}

	void test_uri()
	{
	    Request req;
	    /* extension; unclear to me if the Request-URI can contain spaces */
	    add(req, "GET foo bar baz HTTP/1.1");
	    add(req, "");
	    assert_(req.complete);
	    assert_(!req.broken);
	    assert_eq(req.method, Request::GET);
	    assert_eq(req.request_uri(), "foo bar baz");
	    assert_eq(req.version, Request::HTTP11);
	}

	void test_unknown()
	{
	    Request req;
	    add(req, "FOO foo HTTP/1.2");
	    add(req, "");
	    assert_(req.complete);
	    assert_(!req.broken);
	    assert_eq(req.method, Request::UNKNOWN);
	    assert_eq(req.request_uri(), "foo");
	    assert_eq(req.version, Request::UNKNOWN);
	}
    }
}
