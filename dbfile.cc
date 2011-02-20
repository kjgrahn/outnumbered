/* $Id: dbfile.cc,v 1.4 2011-02-20 19:55:00 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "dbfile.h"

#include <gdbm.h>
#include <cstdlib>
#include <cstring>
#include <errno.h>

namespace {

    datum datumstring(const std::string& s)
    {
	datum d;
	d.dptr = const_cast<char*>(s.data());
	d.dsize = s.size();
	return d;
    }

    /**
     * Because gdbm does that stupid mistake of not
     * forward-declaring structs, it's either dealing
     * with void* here or poisoning /our/ header file
     * with /theirs/.
     */
    static GDBM_FILE dbf(void* p)
    {
	return static_cast<GDBM_FILE>(p);
    }
}


/**
 * Open a db.
 * Uses gdbm_open(file, 0, WRCREAT | NOLOCK, mode, NULL).
 */
DbFile::DbFile(const std::string& file, int mode)
    : db_(gdbm_open(const_cast<char*>(file.c_str()), 0,
		    GDBM_WRCREAT | GDBM_NOLOCK,
		    mode, 0))
{}


DbFile::~DbFile()
{
    close();
}


void DbFile::close()
{
    if(db_) {
	/* Funny that they don't report errors here ...
	 * possibly one should sync to disk first.
	 */
	gdbm_close(dbf(db_));
	db_ = 0;
    }
}


/**
 * Supposed to be, immediately after a failure, a text describing
 * said failure -- either from gdbm or (if gdbm_errno is clear)
 * plain old strerror(errno).
 *
 * The gdbm manual is unfortunately unclear on under which conditions
 * this is valid.
 */
std::string DbFile::strerror() const
{
    std::string s;
    if(gdbm_errno) {
	s = gdbm_strerror(gdbm_errno);
    }
    else {
	s = std::strerror(errno);
    }
    return s;
}


bool DbFile::insert(const std::string& key, const std::string& val)
{
    if(val.empty()) return false;
    return gdbm_store(dbf(db_),
		      datumstring(key), datumstring(val),
		      GDBM_INSERT)==0;
}


bool DbFile::replace(const std::string& key, const std::string& val)
{
    if(val.empty()) return false;
    return gdbm_store(dbf(db_),
		      datumstring(key), datumstring(val),
		      GDBM_REPLACE)==0;
}


bool DbFile::has(const std::string& key)
{
    return gdbm_exists(dbf(db_), datumstring(key));
}


std::string DbFile::get(const std::string& key)
{
    std::string s;

    /* It's unfortunate that the only way to read a gdbm involves
     * getting a malloced buffer back. Would it have killed them
     * to provide "write into a user-provided buffer, and complain
     * if it's too small?"
     */
    const datum d = gdbm_fetch(dbf(db_), datumstring(key));
    if(d.dptr) {
	s.assign(d.dptr, d.dsize);
	std::free(d.dptr);
    }
    return s;
}
