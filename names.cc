/* Copyright (c) 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "names.h"

#include <sstream>

namespace {

    const Request::Property init[] = {
	Request::OPTIONS,
	Request::GET,
	Request::HEAD,
	Request::POST,
	Request::PUT,
	Request::DELETE,
	Request::TRACE,
	Request::CONNECT,

	Request::HTTP10,
	Request::HTTP11,

	Request::Request_URI,

	Request::Cache_Control,
	Request::Connection,
	Request::Date,
	Request::Pragma,
	Request::Trailer,
	Request::Transfer_Encoding,
	Request::Upgrade,
	Request::Warning,
	Request::Via,

	Request::Accept_Charset,
	Request::Authorization,
	Request::If_Match,
	Request::Accept_Encoding,
	Request::Expect,
	Request::If_Modified_Since,
	Request::Accept_Language,
	Request::From,
	Request::If_None_Match,
	Request::Accept,
	Request::Host,
	Request::If_Range,
	Request::If_Unmodified_Since,
	Request::Max_Forwards,
	Request::Proxy_Authorization,
	Request::Range,
	Request::Referer,
	Request::TE,
	Request::User_Agent,
	Request::Accept_Ranges,
	Request::Age,
	Request::Content_Disposition,
	Request::ETag,
	Request::Location,
	Request::Proxy_Authenticate,
	Request::Retry_After,
	Request::Server,
	Request::Vary,
	Request::WWW_Authenticate,
	Request::Content_Encoding,
	Request::Allow,
	Request::Content_Language,
	Request::Expires,
	Request::Content_Length,
	Request::Last_Modified,
	Request::Content_Location,
	Request::Content_MD5,
	Request::Content_Range,
	Request::Content_Type,
    };
}


/**
 * Look up [a..b), case-insensitively (XXX not implemented yet).
 * If the lookup fails UNKNOWN is returned.
 */
Request::Property Names::lookup(const char* a, const char* b) const
{
    const std::string s(a, b);
    Map::const_iterator i = map.find(s);
    if(i==map.end()) {
	return Request::UNKNOWN;
    }
    else {
	return i->second;
    }
}


Names::Names()
{
    const Request::Property* p = init;
    const Request::Property* const q = p + sizeof init / sizeof init[0];

    while(p!=q) {
	std::ostringstream oss;
	oss << *p;
	map[oss.str()] = *p;
	p++;
    }
}


std::ostream& operator<< (std::ostream& os, const Request::Property& val)
{
    const char* s;
    switch(val) {
    case Request::OPTIONS: s = "OPTIONS"; break;
    case Request::GET:     s = "GET"; break;
    case Request::HEAD:    s = "HEAD"; break;
    case Request::POST:    s = "POST"; break;
    case Request::PUT:     s = "PUT"; break;
    case Request::DELETE:  s = "DELETE"; break;
    case Request::TRACE:   s = "TRACE"; break;
    case Request::CONNECT: s = "CONNECT"; break;

    case Request::HTTP10: s = "HTTP/1.0"; break;
    case Request::HTTP11: s = "HTTP/1.1"; break;

    case Request::Request_URI: s = "Request-URI"; break;

    default: s = "unknown"; break;

    /* actual RFC 2616 headers */
    case Request::Cache_Control: s = "Cache-Control"; break;
    case Request::Connection: s = "Connection"; break;
    case Request::Date: s = "Date"; break;
    case Request::Pragma: s = "Pragma"; break;
    case Request::Trailer: s = "Trailer"; break;
    case Request::Transfer_Encoding: s = "Transfer-Encoding"; break;
    case Request::Upgrade: s = "Upgrade"; break;
    case Request::Via: s = "Via"; break;
    case Request::Warning: s = "Warning"; break;

    case Request::Accept: s = "Accept"; break;
    case Request::Accept_Charset: s = "Accept-Charset"; break;
    case Request::Accept_Encoding: s = "Accept-Encoding"; break;
    case Request::Accept_Language: s = "Accept-Language"; break;
    case Request::Authorization: s = "Authorization"; break;
    case Request::Expect: s = "Expect"; break;
    case Request::From: s = "From"; break;
    case Request::Host: s = "Host"; break;
    case Request::If_Match: s = "If-Match"; break;
    case Request::If_Modified_Since: s = "If-Modified-Since"; break;
    case Request::If_None_Match: s = "If-None-Match"; break;
    case Request::If_Range: s = "If-Range"; break;
    case Request::If_Unmodified_Since: s = "If-Unmodified-Since"; break;
    case Request::Max_Forwards: s = "Max-Forwards"; break;
    case Request::Proxy_Authorization: s = "Proxy-Authorization"; break;
    case Request::Range: s = "Range"; break;
    case Request::Referer: s = "Referer"; break;
    case Request::TE: s = "TE"; break;
    case Request::User_Agent: s = "User-Agent"; break;

    case Request::Accept_Ranges: s = "Accept-Ranges"; break;
    case Request::Age: s = "Age"; break;
    case Request::ETag: s = "ETag"; break;
    case Request::Location: s = "Location"; break;
    case Request::Proxy_Authenticate: s = "Proxy-Authenticate"; break;
    case Request::Retry_After: s = "Retry-After"; break;
    case Request::Server: s = "Server"; break;
    case Request::Vary: s = "Vary"; break;
    case Request::WWW_Authenticate: s = "WWW-Authenticate"; break;

    case Request::Allow: s = "Allow"; break;
    case Request::Content_Encoding: s = "Content-Encoding"; break;
    case Request::Content_Language: s = "Content-Language"; break;
    case Request::Content_Length: s = "Content-Length"; break;
    case Request::Content_Location: s = "Content-Location"; break;
    case Request::Content_MD5: s = "Content-MD5"; break;
    case Request::Content_Range: s = "Content-Range"; break;
    case Request::Content_Type: s = "Content-Type"; break;
    case Request::Expires: s = "Expires"; break;
    case Request::Last_Modified: s = "Last-Modified"; break;
    }
    return os << s;
}
