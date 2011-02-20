/* -*- c++ -*-
 * $Id: dbfile.h,v 1.3 2011-02-20 09:52:36 grahn Exp $
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
 * and assumed to be fairly small but non-empty.
 *
 */
class DbFile {
public:
    DbFile(const std::string& file, int mode=0777);
    ~DbFile();

    void close();

    bool bad() const;
    std::string strerror() const;

    bool insert(const std::string& key, const std::string& val);
    bool replace(const std::string& key, const std::string& val);
    bool has(const std::string& key);
    std::string get(const std::string& key);

private:
    DbFile();
    DbFile(const DbFile&);
    DbFile& operator= (const DbFile&);
};

#endif
