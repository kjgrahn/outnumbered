/**
 * $Id: version.cc,v 1.2 2010-07-18 18:59:01 grahn Exp $
 *
 * Copyright (c) 2010 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "version.h"
#include <algorithm>

std::string version()
{
    static const char dollar_name[] = "$Name:  $";
    static const char UNKNOWN[] = "non-release version";

    std::string name = dollar_name;
    std::string::iterator a = name.begin();
    const std::string::iterator e = name.end();
    a = std::find(a, e, ' ');
    if(a==e) return UNKNOWN;
    ++a;
    std::string::iterator b = std::find(a, e, ' ');
    if(b==e) return UNKNOWN;
    if(b==a) return UNKNOWN;

    /* [a, b) is now the second, non-empty word.
     * If it looks kind of like gresabladet-1-14-3,
     * return "1.14.3".
     */
    a = std::find(a, b, '-');
    if(a==b) return UNKNOWN;
    ++a;
    std::replace(a, b, '-', '.');
    return std::string(a, b);
}
