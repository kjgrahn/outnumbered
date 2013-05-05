/* Copyright (c) 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "request.h"

#include "names.h"

#include <cassert>
#include <cctype>
#include <limits>
#include <iostream>


namespace {

    const char* ws(const char* a, const char* b)
    {
	while(a!=b && std::isspace(*a)) a++;
	return a;
    }

    const char* non_ws(const char* a, const char* b)
    {
	while(a!=b && !std::isspace(*a)) a++;
	return a;
    }

    /**
     * Advance through [a, b) until a non-token [2.2] is found.
     */
    const char* token(const char* a, const char* b)
    {
	while(a!=b) {
	    const char c = *a;
	    if(0 <= c && c <= 31) return a;
	    if(c==127) return a;
	    switch(c) {
	    case  '(':
	    case  ')':
	    case  '<':
	    case  '>':
	    case  '@':
	    case  ',':
	    case  ';':
	    case  ':':
	    case '\\':
	    case  '"':
	    case  '/':
	    case  '[':
	    case  ']':
	    case  '?':
	    case  '=':
	    case  '{':
	    case  '}':
	    case  ' ':
	    case '\t':
		return a;
	    }
	    a++;
	}
	return a;
    }

    /**
     * Trim whitespace to the right in [a, b), so that
     * it's either empty or ends with non-whitespace.
     */
    const char* trimr(const char* a, const char* b)
    {
	while(a!=b && std::isspace(*(b-1))) b--;
	return b;
    }

    Names names;
}


Request::Request()
    : complete(false),
      broken(false),
      method(END),
      version(UNKNOWN)
{}


void Request::add(const char* a, const char* b)
{
    if(method==END) {
	first_line(a, b);
    }
    else {
	const char* p = ws(a, b);

	if(p==b) {
	    end_line(a, b);
	}
	else if(p==a || properties.size()==1) {
	    plain_line(a, b);
	}
	else {
	    cont_line(a, b);
	}
    }
}


/**
 * A whitespace-only line, after at least first_line().
 */
void Request::first_line(const char* a, const char* const b)
{
    /* \s* (\S+) \s+ (.+) \s+ http/(\S+) \s* */
    a = ws(a, b);
    const char* const meth = a;
    a = non_ws(a, b);

    method = names.lookup(meth, a);

    a = ws(a, b);
    const char* const uri = a;
    const char* p = a;
    const char* q = a;

    while(a!=b) {
	p = q;
	q = a;
	a = non_ws(a, b);
	a = ws(a, b);
    }

    version = names.lookup(q, non_ws(q, b));
    insert(Request_URI, uri, non_ws(p, b));
}


/**
 * A plain header line, not a continuation line. The syntax here is
 *
 *   name : value
 *
 * with whitespace allowed anywhere except in 'name'.
 */
void Request::plain_line(const char* a, const char* b)
{
    a = ws(a, b);
    const char* const name = a;
    a = token(a, b);
    const Property prop = names.lookup(name, a);
    /* If prop==UNKNOWN, we could store an empty entry now.  We must
     * store /something/, in case a continuation line follows.
     */
    a = ws(a, b);
    if(a==b || *a!=':') {
	broken = true;
    }
    else {
	a++;
	a = ws(a, b);
	b = trimr(a, b);

	if(!properties.empty() && properties.back().prop == prop) {
	    /* append to previous value as "old, new" */
	    static const char delim[] = ", ";
	    v.insert(v.end(), delim, delim+2);
	    v.insert(v.end(), a, b);
	}
	else {
	    insert(prop, a, b);
	}
    }
}


/**
 * A continuation header line: starts with whitespace, is not the end
 * line, and there is an preceding header to continue filling.
 */
void Request::cont_line(const char* a, const char* b)
{
    a = ws(a, b) - 1;
    b = trimr(a, b);

    v.insert(v.end(), a, b);
}


/**
 * A whitespace-only line, after at least first_line().
 * Completes the Request.
 */
void Request::end_line(const char*, const char* const)
{
    complete = true;
    broken = broken || v.size() > std::numeric_limits<unsigned short>::max();
    insert(END, 0, 0);
}


void Request::insert(Property prop,
		     const char* a, const char* const b)
{
    unsigned short n = v.size();
    v.insert(v.end(), a, b);
    properties.push_back(Entry(prop, n));
}


/**
 * Return the value of the first occurence of the header 'prop' or the
 * special Request-URI.  Undefined results if you ask for GET, HTTP10
 * or something.  Undefined results if not complete or broken.
 *
 * Continuation lines [2.2] are taken into account, i.e. the
 * continuation is included in the result.
 *
 * Multiple occurencies of a certain header [4.2] are kind of
 * supported; their values are combined into "val, val, ..." if they
 * appear in series. This is slighly incorrect, since it applies to
 * headers which may not be duplicated, and since the ones who /can/
 * don't have to come in one unbroken series.
 */
Blob Request::header(const Property prop) const
{
    assert(complete && !broken);
    const char* p = &v[0];
    for(unsigned i=0; i<properties.size()-1; i++) {
	if(properties[i].prop==prop) {
	    return Blob(p + properties[i].start,
			properties[i+1].start - properties[i].start);
	}
    }
    return Blob();
}


/**
 * Return the Request-URI. Undefined unless complete.
 */
std::string Request::request_uri() const
{
    assert(complete);
    return std::string(&v[properties[0].start],
		       &v[properties[1].start]);
}


/**
 * Debug-print a (complete) request as
 *
 *   METHOD(uri, version) {
 *     header: value,
 *     header: value,
 *     ...
 *   }
 *
 * The printing can never be perfect, since a Request doesn't retain
 * all information, e.g. unknown headers.
 */
std::ostream& Request::put(std::ostream& os) const
{
    if(broken || !complete) {
	return os << method;
    }

    const char* p = &v[0];

    os << method << '(' << request_uri() << ", " << version << ") {\n";
    for(unsigned i=1; i<properties.size()-1; i++) {
	os << properties[i].prop << ": ";
	os.write(p + properties[i].start,
		 properties[i+1].start - properties[i].start);
	os << ",\n";
    }
    return os << '}';
}
