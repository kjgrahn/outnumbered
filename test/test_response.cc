/*
 * $Id: test_response.cc,v 1.2 2010-07-29 09:56:19 grahn Exp $
 *
 * Copyright (C) 2007 Jörgen Grahn.
 * All rights reserved.
 */
#include <response.h>

#include <string>
#include <sstream>
#include <testicle.h>

namespace {

    template<class T>
    struct Printable {
	explicit Printable(const T& val)
	    : val_(val)
	{}
	T val_;
    };

    template<class T>
    std::ostream& operator<< (std::ostream& os, const Printable<T>& val)
    {
	return os << val.val_;
    }

    void assert_eq(const Response& resp, const char* s)
    {
    }

}

namespace response {

    void test()
    {
	const Response r(314);
	assert_eq(r, "314\r\n");
    }

    void test_simple()
    {
	Response r(314);
	r << 1 << 2 << 16;
	r << ".foo";
	assert_eq(r, "314 1 2 16 .foo\r\n");
    }

    void test_simple2()
    {
	Response r(314);
	r << 1 << 2 << 16;
	r << std::string(".foo");
	assert_eq(r, "314 1 2 16 .foo\r\n");
    }

    void test_simple3()
    {
	Response r(314);
	r << 1 << 2 << Printable(16);
	r << Printable(".foo");
	assert_eq(r, "314 1 2 16 foo\r\n");
    }

    void test_multiline1()
    {
	Response r(314);
	r << 1 << 2 << ".foo" << CRLF;
	assert_eq(r,
		  "314 1 2 .foo\r\n"
		  ".\r\n");
    }

    void test_multiline1()
    {
	Response r(314);
	r << 1 << 2 << ".foo" << CRLF;
	r << "bar" << CRLF
	  << 42 << "baz" << CRLF;
	assert_eq(r,
		  "314 1 2 .foo\r\n"
		  "bar\r\n"
		  "42 baz\r\n"
		  ".\r\n");
    }

    void test_multiline2()
    {
	Response r(314);
	r << 1 << 2 << ".foo" << CRLF;
	r << "bar" << CRLF
	  << ".fred" << "baz" << CRLF;
	assert_eq(r,
		  "314 1 2 .foo\r\n"
		  "bar\r\n"
		  "..fred baz\r\n"
		  ".\r\n");
    }

}
