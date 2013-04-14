/*
 * Copyright (C) 2013 Jörgen Grahn.
 * All rights reserved.
 */
#include <deflate.h>

#include <testicle.h>
#include <cstdlib>

namespace {

    void push(std::vector<uint8_t>& dest,
	      Deflate& defl,
	      const std::string& src)
    {
	defl.push(Blob(src));
	const Blob out = defl.front();
	if(out) {
	    dest.insert(dest.end(), out.begin(), out.end());
	    defl.pop();
	}
    }

    void finish(std::vector<uint8_t>& dest,
		Deflate& defl)
    {
	defl.finish();
	const Blob out = defl.front();
	if(out) {
	    dest.insert(dest.end(), out.begin(), out.end());
	    defl.pop();
	}
    }

}

namespace deflate {

    void test_hello()
    {
	Deflate d;
	std::vector<uint8_t> dest;
	push(dest, d, "Hello, world!");
	finish(dest, d);
	testicle::assert_gt(dest.size(), 0);
    }

    void test_hellos()
    {
	Deflate d;
	std::vector<uint8_t> dest;
	for(int i=0; i<20000; i++) {
	    push(dest, d,
		 "Hello, world!\n"
		 "Hello, world!\n"
		 "Hello, world!\n"
		 "Hello, world!\n"
		 "Hello, world!\n");
	}
	finish(dest, d);
	testicle::assert_gt(dest.size(), 0);
	testicle::assert_lt(dest.size(), .1 * 20e3*14*5);
    }

    void test_compresses_well()
    {
	Deflate d;
	std::vector<uint8_t> dest;
	for(int i=0; i<20000; i++) {
	    push(dest, d,
		 "00000000000000"
		 "00000000000000"
		 "00000000000000"
		 "00000000000000"
		 "00000000000000");
	}
	finish(dest, d);
	testicle::assert_gt(dest.size(), 0);
	testicle::assert_lt(dest.size(), .1 * 20e3*14*5);
    }

    void test_compresses_badly()
    {
	Deflate d;
	std::vector<uint8_t> dest;
	std::string src(100, 'x');
	for(int i=0; i<20000; i++) {
	    for(int j=0; j<100; j++) {
		src[j] = std::rand();
	    }
	    push(dest, d, src);
	}
	finish(dest, d);
	testicle::assert_gt(dest.size(), .9 * 20e3*100);
    }
}
