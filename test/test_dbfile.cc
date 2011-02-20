/*
 * $Id: test_dbfile.cc,v 1.1 2011-02-20 08:47:31 grahn Exp $
 *
 * Copyright (C) 2011 Jörgen Grahn.
 * All rights reserved.
 */
#include <dbfile.h>

#include <string>
#include <stdio.h>
#include <testicle.h>

namespace {

    std::string tmpname() { return tmpnam(0); }

    /**
     * Maintains a temporary DbFile where the
     * keys are "0", "1", ... size, and the data
     * for key N is N octets long.
     */
    struct Corpus {
	explicit Corpus(int size);
	~Corpus();
	std::string filename;
	DbFile db;
    };

    /**
     * The standard key #n.
     */
    std::string key(int n)
    {
	char key[10];
	std::sprintf(key, "%d", n);
	return key;
    }

    /**
     * The standard value for key #n.
     */
    std::string val(int n)
    {
	std::string s;
	while(n>0) {
	    s.push_back(static_cast<char>(n));
	    --n;
	}
	return s;
    }

    Corpus::Corpus(int size)
	: filename(tmpname()),
	  db(filename, 0600)
    {
	for(int n=0; n<size; ++n) {
	}
    }
}


namespace db {

    namespace open {
    }

    namespace read {
    }

    namespace write {
    }

}
