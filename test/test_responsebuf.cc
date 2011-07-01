/*
 * $Id: test_responsebuf.cc,v 1.3 2011-07-01 07:41:56 grahn Exp $
 *
 * Copyright (C) 2011 Jörgen Grahn.
 * All rights reserved.
 */
#include <responsebuf.h>

#include <testicle.h>

using namespace testicle;

namespace responsebuf {

    void test_construct()
    {
	ResponseBuf buf(-1);
	assert_eq(buf.empty(), true);
	assert_eq(buf.str(), "");
    }

    void test_simple1()
    {
	ResponseBuf buf(-1);
	buf << "foo\r\n";
	assert_eq(buf.empty(), false);
	assert_eq(buf.str(), "foo\r\n");
    }

    void test_simple2()
    {
	ResponseBuf buf(-1);
	buf.ostream().put('f').write("oo\r\n", 4);
	assert_eq(buf.empty(), false);
	assert_eq(buf.str(), "foo\r\n");
    }

    void test_flush()
    {
	ResponseBuf buf(-1);
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
	    ResponseBuf buf(-1);
	    buf << "200 hello.\r\n";
	    buf.write_termination();
	    assert_eq(buf.empty(), false);
	    assert_eq(buf.str(),
		      "200 hello.\r\n"
		      ".\r\n");
	}

	void test_simple()
	{
	    ResponseBuf buf(-1);
	    buf << "200 hello.\r\n"
		<< ".world\r\n";
	    buf.write_termination();
	    assert_eq(buf.empty(), false);
	    assert_eq(buf.str(),
		      "200 hello.\r\n"
		      "..world\r\n"
		      ".\r\n");
	}

	void test_sloppy()
	{
	    ResponseBuf buf(-1);
	    buf << "200 hello.\r\n"
		<< ".world";
	    buf.write_termination();
	    assert_eq(buf.empty(), false);
	    assert_eq(buf.str(),
		      "200 hello.\r\n"
		      "..world\r\n"
		      ".\r\n");
	}

	void test_twice()
	{
	    ResponseBuf buf(-1);
	    buf << 200 << " hello.\r\n"
		<< ".foo\r\n"
		<< ".bar\r\n"
		<< ".baz\r\n"
		<< ".bat\r\n";
	    buf.write_termination();
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
}
