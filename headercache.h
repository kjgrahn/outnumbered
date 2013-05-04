/* -*- c++ -*-
 *
 * Copyright (c) 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_HCACHE_H_
#define GB_HCACHE_H_

#include <tr1/unordered_map>
#include <string>

/**
 * A cache of objects parsed from strings.
 *
 * Many strings appear over and over again in requests, and parse in
 * the same way independent of context; for example the different
 * Accept-*: headers.  If you've parsed such a string once you don't
 * have to do it again, just pick up the previous result.
 *
 * If the cache's size reaches a threshold, it's simply cleared --
 * there's no aging mechanism.
 *
 * It turns out the nginx web server used the same technique
 *   http://nginx.org/en/docs/hash.html
 * but I invented it independently.  Perhaps it's an obvious idea.
 *
 */
template <class T>
class HeaderCache {
public:
    const T& insert(const std::string& s);
    const T& insert(const char* a, const char* b);

private:
    typedef std::tr1::unordered_map<std::string, T> Map;
    static const size_t THRESHOLD = 50;
    Map map;
};


template <class T>
const T& HeaderCache<T>::insert(const std::string& s)
{
    typename Map::iterator i = map.find(s);
    if(i!=map.end()) {
	return i->second;
    }
    else {
	if(map.size()==THRESHOLD) map.clear();
	const char* p = s.c_str();
	const T t(p, p+s.size());
	return map[s] = t;
    }
}


template <class T>
const T& HeaderCache<T>::insert(const char* a, const char* b)
{
    const std::string s(a, b-a);
    return insert(s);
}

#endif
