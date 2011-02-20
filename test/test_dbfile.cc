/*
 * $Id: test_dbfile.cc,v 1.4 2011-02-20 11:03:55 grahn Exp $
 *
 * Copyright (C) 2011 Jörgen Grahn.
 * All rights reserved.
 */
#include <dbfile.h>

#include <string>
#include <stdio.h>
#include <unistd.h>

#include <testicle.h>

namespace {

    std::string tmpname() { return tmpnam(0); }

    /**
     * Maintains a temporary DbFile where the
     * keys are "0", "1", ... size, and the data
     * for key N is N+1 octets long.
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
	std::string s("#");
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
	if(db.bad()) throw "XXX";

	for(int n=0; n<size; ++n) {
	    if(!db.insert(key(n), val(n))) throw "XXX";
	}
    }


    Corpus::~Corpus()
    {
	db.close();
	unlink(filename.c_str());
    }

}


namespace db {

    namespace open {

	void test_notfile()
	{
	    DbFile db("/");
	    testicle::assert_(db.bad());
	    testicle::assert_neq(db.strerror(), "");
	}

	void test_openclose()
	{
	    std::string filename = tmpname();

	    DbFile a(filename);
	    testicle::assert_(!a.bad());
	    testicle::assert_(a.insert("foo", "bar"));
	    testicle::assert_eq(a.get("foo"), "bar");
	    a.close();

	    DbFile b(filename);
	    testicle::assert_eq(b.get("foo"), "bar");
	    b.close();

	    unlink(filename.c_str());
	}
    }

    namespace readwrite {

	void test_read()
	{
	    const int size = 10;
	    Corpus corpus(size);
	    DbFile& db = corpus.db;

	    testicle::assert_eq(db.get(key(size)), "");
	    for(int i=0; i<size; ++i) {
		testicle::assert_eq(db.get(key(i)), val(i));
	    }
	}

	void test_simple()
	{
	    Corpus corpus(10);
	    DbFile& db = corpus.db;

	    testicle::assert_(db.insert("foo", "bar"));
	    testicle::assert_eq(db.get("foo"), "bar");
	    testicle::assert_(db.has("foo"));
	}

	void test_replace()
	{
	    Corpus corpus(10);
	    DbFile& db = corpus.db;

	    const std::string val("Hello, world!");
	    testicle::assert_(db.has("5"));
	    testicle::assert_(!db.has("x"));
	    testicle::assert_(db.replace("5", val));
	    testicle::assert_(db.replace("x", val));
	    testicle::assert_eq(db.get("5"), val);
	    testicle::assert_eq(db.get("x"), val);
	}

	void test_noemptyval()
	{
	    Corpus corpus(10);
	    DbFile& db = corpus.db;

	    testicle::assert_(!db.has("foo"));
	    testicle::assert_(!db.insert("foo", ""));
	    testicle::assert_eq(db.get("foo"), "");
	    testicle::assert_(!db.has("foo"));
	}

	void test_huge()
	{
	    Corpus corpus(10);
	    DbFile& db = corpus.db;

	    const std::string key(1e3, 'k');
	    const std::string val(20e3, 'x');
	    testicle::assert_(!db.has(key));
	    testicle::assert_(db.insert(key, val));
	    testicle::assert_eq(db.get(key), val);
	    testicle::assert_(db.has(key));
	}

	void test_nul()
	{
	    Corpus corpus(10);
	    DbFile& db = corpus.db;

	    std::string key("key");
	    std::string val("val");
	    key[1] = val[1] = '\0';
	    testicle::assert_(!db.has(key));
	    testicle::assert_(db.insert(key, val));
	    testicle::assert_(db.has(key));
	    testicle::assert_eq(db.get(key), val);
	}
    }
}
