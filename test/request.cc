/*
 * Copyright (C) 2012 Jörgen Grahn.
 * All rights reserved.
 */
#include <request.h>

#include <testicle.h>


namespace req {

    using testicle::assert_;
    using testicle::assert_eq;

    void test_simple()
    {
	Request req;
	req.add("GET /pub/WWW/TheProject.html HTTP/1.1");
	req.add("Host: www.w3.org");
	req.add("");

	assert_(req.complete());
	assert_(!req.broken());
	assert_eq(req.method(), Request::GET);
	assert_eq(req.request_uri(), "/pub/WWW/TheProject.html");
	assert_eq(req.http_version(), Request::HTTP11);
    }

    void test_opera()
    {
	Request req;
	req.add("GET /contentfile/imagecrop/1.7867487?cropid=f169w225 HTTP/1.1");
	req.add("User-Agent: Opera/9.80 (X11; Linux x86_64) Presto/2.12.388 Version/12.11");
	req.add("Host: www.nrk.no");
	req.add("Accept: text/html, application/xml;q=0.9, application/xhtml+xml, "
		"image/png, image/webp, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
	req.add("Accept-Language: en,sv;q=0.9");
	req.add("Accept-Encoding: gzip, deflate");
	req.add("Cache-Control: no-cache");
	req.add("Connection: Keep-Alive");
	req.add("");
    }
}
