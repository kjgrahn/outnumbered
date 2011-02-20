/* -*- c++ -*-
 * $Id: dbfile.h,v 1.2 2011-02-20 08:47:31 grahn Exp $
 *
 * Copyright (c) 2011 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_DBFILE_H_
#define GB_DBFILE_H_

#include <string>

/**
 * A wrapping of gdbm, with only the features needed here.
 * Keys and values are std::string or std::vector<char>,
 * and assumed to be fairly small.
 *
 */
class DbFile {
public:
    DbFile(const std::string& file, int mode);

private:
    DbFile();
    DbFile(const DbFile&);
    DbFile& operator= (const DbFile&);

};

#endif
