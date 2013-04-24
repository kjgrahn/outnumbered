/* -*- c++ -*-
 *
 * Copyright (c) 2012, 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_REQUEST_H_
#define GB_REQUEST_H_

#include "blob.h"

#include <iosfwd>
#include <string>
#include <vector>


/**
 * A HTTP request, or what should be one, as received and parsed.
 *
 * RFC 2616, section [5]. A request is
 *
 *   method SP Request-URI SP HTTP/n.n CRLF
 *   header CRLF
 *   header CRLF
 *   ...
 *   CRLF
 *   [body]
 *
 * It's implemented as a state machine, which moves forward by being
 * fed line by line until complete(), at which point it may also be
 * broken().
 *
 * Since we don't support POST, this class is optimized for fairly
 * short requests, with no extremely long message body.
 *
 * The headers, and the URI, are stored in a single vector<char>,
 * together with an array
 *   property: offset
 *   property: offset
 *   ...
 *   end: offset
 */
class Request {
public:

    enum Property {
	UNKNOWN,
	END,

	/* request method [5.1.1] */
	OPTIONS,  GET,     HEAD,
	POST,     PUT,     DELETE,
	TRACE,    CONNECT,

	/* HTTP version [3.1] */
	HTTP10, HTTP11,

	Request_URI,

	/* general-header [4.5] */
	Cache_Control,
	Connection,
	Date,
	Pragma,
	Trailer,
	Transfer_Encoding,
	Upgrade,
	Warning,
	Via,

	/* request-header[5.3] */
	Accept_Charset,	  Authorization,  If_Match,
	Accept_Encoding,  Expect,	  If_Modified_Since,
	Accept_Language,  From,		  If_None_Match,
	Accept,		  Host,		  If_Range,
					  If_Unmodified_Since,
	Max_Forwards,
	Proxy_Authorization,
	Range,
	Referer,
	TE,
	User_Agent,

	/* response-header [6.2], RFC 6266 */
	Accept_Ranges,
	Age,
	Content_Disposition,
	ETag,
	Location,
	Proxy_Authenticate,
	Retry_After,
	Server,
	Vary,
	WWW_Authenticate,

	/* entity-header [7.1] */
	Content_Encoding,  Allow,
	Content_Language,  Expires,
	Content_Length,	   Last_Modified,
	Content_Location,
	Content_MD5,
	Content_Range,
	Content_Type
    };
    typedef Property Method;
    typedef Property HTTPVersion;

    Request();

    void add(const char* a, const char* b);

    bool complete;
    bool broken;
    Method method;
    HTTPVersion version;

    Blob header(Property prop) const;
    std::string request_uri() const;
    std::ostream& put(std::ostream& os) const;

private:
    struct Entry {
	Entry(Property prop, unsigned short start)
	    : prop(prop),
	      start(start)
	{}
	Property prop;
	unsigned short start;
    };
    std::vector<char> v;
    std::vector<Entry> properties;

    void first_line(const char* a, const char* const b);
    void plain_line(const char* a, const char* const b);
    void cont_line(const char* a, const char* const b);
    void end_line(const char* a, const char* const b);

    void insert(Property prop, const char* a, const char* const b);
};

inline
std::ostream& operator<< (std::ostream& os, const Request& val) {
    return val.put(os);
}
std::ostream& operator<< (std::ostream& os, const Request::Property& val);

#endif
