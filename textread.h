/* -*- c++ -*-
 *
 * Copyright (c) 2010, 2012 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef SOCKET_TEXTREAD_H_
#define SOCKET_TEXTREAD_H_

#include <cstdlib>
#include <string>

namespace sockutil {

    /**
     * Reading lines of text from a TCP socket, which may be in
     * blocking or non-blocking mode. Or more generally, reading data
     * with some kind of single- or multi-octet end-of-record marker;
     * "text" is just a special case with '\n' or "\r\n" as that
     * marker.  Useful with protocols like SMTP and HTTP.  Also
     * tolerates '\0' in the stream.
     *
     * Usage:
     *
     * feed() calls recv(2) exactly once (so that it can be used with
     * select(2) and blocking sockets). It may read some stream data
     * or set eof().
     *
     * After feed(), one of the read() functions should be called
     * repeatedly until it returns 0 or the empty string,
     * respectively. Each string returned includes the endline, except
     * possibly the last one on the stream.  Strings longer than the
     * fixed-size internal buffer aren't supported: if one comes it
     * will look like EOF. (Most protocols specify a minimum line length
     * which you have to support.)
     *
     * Whenever eof() is set, there is no point in calling feed()
     * again, but there may be lines left to read() from the buffer.
     *
     * When both eof() and error() are set, error() is the errno which
     * caused feed() to fail, and strerror() is the error text.
     * 
     */
    class TextReader {
    public:
	explicit TextReader(const std::string& endline);
	TextReader(const TextReader&);
	TextReader& operator= (const TextReader&);

	void feed(int fd);

	size_t read(char*& begin, char*& end);
	std::string read();

	bool eof() const { return eof_; }
	int error() const { return errno_; }
	const char* strerror() const;

    private:
	TextReader();

	std::string endline_;
	char buf[8000];
	char* const p_;
	char* const q_;
	char* a_;
	char* b_;
	bool eof_;
	int errno_;
    };

}

#endif
