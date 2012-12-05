/* -*- c++ -*-
 *
 * Copyright (c) 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef GB_REQUEST_H_
#define GB_REQUEST_H_

#include <string>


/**
 * A HTTP request, or what should be one, as recieved and parsed.
 */
class Request {
public:

    enum Property {

	/* request method [5.1.1] */
	OPTIONS, GET, HEAD,
	POST, PUT, DELETE,
	TRACE, CONNECT,

	/* HTTP version [3.1] */
	HTTP10, HTTP11,

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
	Accept_Charset,
	Accept_Encoding,
	Accept_Language,
	Accept,
	Authorization,
	Expect,
	From,
	Host,
	If_Match,
	If_Modified_Since,
	If_None_Match,
	If_Range,
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
	Allow,
	Content_Encoding,
	Content_Language,
	Content_Length,
	Content_Location,
	Content_MD5,
	Content_Range,
	Content_Type,
	Expires,
	Last_Modified
    };

    void add(const std::string& s);

    bool complete() const;
    bool broken() const;

    Method method() const;
    HTTPVersion http_version() const;
    std::string request_uri() const;

};

#endif
