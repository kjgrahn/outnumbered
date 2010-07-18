/**
 * $Id: version.cc,v 1.1 2010-07-18 16:24:16 grahn Exp $
 *
 * Copyright (c) 2009 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "version.h"
#include "sutil.h"

std::string version()
{
    static const char dollar_name[] = "$Name:  $";

    std::vector<std::string> v = util::split(dollar_name);
    if(v.size()!=3) {
	return "non-release version";
    }
    const std::string s(v[1]);
    util::CharSplit split(s.data(), s.data() + s.length(), '-');
    util::CharSplit::const_iterator i = split.begin();
    ++i;
    v.clear();
    while(i!=split.end()) {
	v.push_back(i->str());
	++i;
    }

    return util::join(".", v);
}
