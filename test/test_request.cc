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

    void assert_header(const Request& req, Request::Property prop,
		       const std::string& val)
    {
	const Blob v = req.header(prop);
	testicle::assert_(v);
	testicle::assert_eq(std::string(v.begin(), v.end()), val);
    }

    void assert_no_header(const Request& req, Request::Property prop)
    {
	const Blob v = req.header(prop);
	testicle::assert_(!v);
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
	assert_header(req, Request::Host, "www.w3.org");
	assert_no_header(req, Request::Accept);
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
	assert_eq(req.method, Request::GET);
	assert_eq(req.request_uri(), "/contentfile/imagecrop/1.7867487?cropid=f169w225");
	assert_eq(req.version, Request::HTTP11);
	assert_header(req, Request::Cache_Control, "no-cache");
	assert_header(req, Request::Accept_Language, "en,sv;q=0.9");
	assert_header(req, Request::Connection, "Keep-Alive");
    }

    namespace overflow {

	void test_plain()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    for(int i=0; i<50e3; i++) {
		add(req, "Accept-Encoding: g");
		add(req, "Accept: z");
	    }
	    add(req, "");
	    assert_(req.complete);
	    assert_(req.broken);
	}

	void test_continuation()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    add(req, "Accept-Encoding: gzip,");
	    for(int i=0; i<10e3; i++) {
		add(req, " deflate");
	    }
	    add(req, "");
	    assert_(req.complete);
	    assert_(req.broken);
	}

	void test_combine()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    for(int i=0; i<50e3; i++) {
		add(req, "Accept-Encoding: gzip");
	    }
	    add(req, "");
	    assert_(req.complete);
	    assert_(req.broken);
	}
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

	void test_spacing()
	{
	    Request req;
	    add(req, "  PUT   foo bar  HTTP/1.0  ");
	    add(req, "");
	    assert_(req.complete);
	    assert_(!req.broken);
	    assert_eq(req.method, Request::PUT);
	    assert_eq(req.request_uri(), "foo bar");
	    assert_eq(req.version, Request::HTTP10);
	}
    }

    namespace header {

	void test_simple()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    add(req, "Accept-Encoding: gzip, deflate");
	    add(req, "");
	    assert_(req.complete); assert_(!req.broken);

	    assert_header(req, Request::Accept_Encoding, "gzip, deflate");
	}

	void test_spacing()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    add(req, "Accept-Encoding :   gzip, deflate   ");
	    add(req, "");
	    assert_(req.complete); assert_(!req.broken);

	    assert_header(req, Request::Accept_Encoding, "gzip, deflate");
	}

	void test_unknown()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    add(req, "Foo: bar");
	    add(req, "Accept-Encoding: gzip, deflate");
	    add(req, "Foo: bar");
	    add(req, "");
	    assert_(req.complete); assert_(!req.broken);

	    assert_header(req, Request::Accept_Encoding, "gzip, deflate");
	}

	void test_empty()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    add(req, "Accept:");
	    add(req, "Accept-Encoding: gzip, deflate");
	    add(req, "Accept:");
	    add(req, "");
	    assert_(req.complete); assert_(!req.broken);

	    assert_header(req, Request::Accept_Encoding, "gzip, deflate");
	}

	void test_nonheader()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    add(req, "Accept:");
	    add(req, "Accept-Encoding = gzip, deflate");
	    add(req, "Accept:");
	    add(req, "");
	    assert_(req.complete);
	    assert_(req.broken);
	}

	void test_unknown_cont()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    add(req, "Foo: bar");
	    add(req, "     baz");
	    add(req, "Accept-Encoding: gzip, deflate");
	    add(req, "Foo: bar");
	    add(req, "     baz");
	    add(req, "");
	    assert_(req.complete); assert_(!req.broken);

	    assert_header(req, Request::Accept_Encoding, "gzip, deflate");
	}

	void test_continuation()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    add(req, "Accept-Encoding: gzip,   ");
	    add(req, "                 deflate ");
	    add(req, "Connection:      Keep-Alive");
	    add(req, "");
	    assert_(req.complete); assert_(!req.broken);

	    assert_header(req, Request::Accept_Encoding, "gzip, deflate");
	    assert_header(req, Request::Connection, "Keep-Alive");
	}

	void test_pseudo_cont()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    /* probably illegal, but harmless to support */
	    add(req, "  Accept-Encoding: gzip, deflate");
	    add(req, "");
	    assert_(req.complete); assert_(!req.broken);

	    assert_header(req, Request::Accept_Encoding, "gzip, deflate");
	}

	void test_combine()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    add(req, "Accept-Encoding: gzip");
	    add(req, "Accept-Encoding: deflate");
	    add(req, "Accept-Encoding: foo");
	    add(req, "Connection:      Keep-Alive");
	    add(req, "");
	    assert_(req.complete); assert_(!req.broken);

	    assert_header(req, Request::Connection, "Keep-Alive");
	    assert_header(req, Request::Accept_Encoding, "gzip, deflate, foo");
	}

	void test_combine_tricky()
	{
	    Request req;
	    add(req, "GET foo HTTP/1.1");
	    add(req, "Accept-Encoding: gzip");
	    add(req, "Accept-Encoding: deflate");
	    add(req, "Connection:      Keep-Alive");
	    add(req, "Accept-Encoding: foo");
	    add(req, "");
	    assert_(req.complete); assert_(!req.broken);

	    assert_header(req, Request::Connection, "Keep-Alive");
	    /* documented deviation from the RFC */
	    assert_header(req, Request::Accept_Encoding, "gzip, deflate");
	}
    }
}
