/*
 * $Id: test_responsebuf.cc,v 1.6 2011-07-03 08:27:21 grahn Exp $
 *
 * Copyright (C) 2011 Jörgen Grahn.
 * All rights reserved.
 */
#include <responsebuf.h>

#include <testicle.h>

using namespace testicle;


/**
 * XXX These all use ResponseBuf::str() which is a pure debug
 * interface, instead of actually writing to a socket.
 */
namespace responsebuf {

    void test_construct()
    {
	ResponseBuf buf;
	assert_eq(buf.empty(), true);
	assert_eq(buf.str(), "");
    }

    void test_simple1()
    {
	ResponseBuf buf;
	buf << "foo\r\n";
	assert_eq(buf.empty(), false);
	assert_eq(buf.str(), "foo\r\n");
    }

    void test_simple2()
    {
	ResponseBuf buf;
	buf.ostream().put('f').write("oo\r\n", 4);
	assert_eq(buf.empty(), false);
	assert_eq(buf.str(), "foo\r\n");
    }

    void test_flush()
    {
	ResponseBuf buf;
	std::ostream& os = buf.ostream();
	os.put('f');
	os.flush();
	os.write("oo\r\n", 4);
	assert_eq(buf.empty(), false);
	assert_eq(buf.str(), "foo\r\n");
    }

    namespace dot {

	void test_none()
	{
	    ResponseBuf buf;
	    buf << "200 hello.\r\n";
	    buf.put_terminator();
	    assert_eq(buf.empty(), false);
	    assert_eq(buf.str(),
		      "200 hello.\r\n"
		      ".\r\n");
	}

	void test_simple()
	{
	    ResponseBuf buf;
	    buf << "200 hello.\r\n"
		<< ".world\r\n";
	    buf.put_terminator();
	    assert_eq(buf.empty(), false);
	    assert_eq(buf.str(),
		      "200 hello.\r\n"
		      "..world\r\n"
		      ".\r\n");
	}

	void test_sloppy()
	{
	    ResponseBuf buf;
	    buf << "200 hello.\r\n"
		<< ".world";
	    buf.put_terminator();
	    assert_eq(buf.empty(), false);
	    assert_eq(buf.str(),
		      "200 hello.\r\n"
		      "..world\r\n"
		      ".\r\n");
	}

	void test_twice()
	{
	    ResponseBuf buf;
	    buf << 200 << " hello.\r\n"
		<< ".foo\r\n"
		<< ".bar\r\n"
		<< ".baz\r\n"
		<< ".bat\r\n";
	    buf.put_terminator();
	    assert_eq(buf.empty(), false);
	    assert_eq(buf.str(),
		      "200 hello.\r\n"
		      "..foo\r\n"
		      "..bar\r\n"
		      "..baz\r\n"
		      "..bat\r\n"
		      ".\r\n");
	}
    }

    namespace huge {

	void test(size_t n)
	{
	    ResponseBuf buf;
	    std::ostringstream oss;
	    for(size_t i=0; i<n; ++i) {
		buf << "Foo bar baz " << i << "\r\n";
		oss << "Foo bar baz " << i << "\r\n";
	    }

	    assert_eq(buf.size(), oss.str().size());
	    assert_eq(buf.str(), oss.str());
	}

	void test() { test(50e3); }
    }
}
