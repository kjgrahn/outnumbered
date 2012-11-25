# $Id: Makefile,v 1.23 2011-07-03 19:17:25 grahn Exp $
#
# Makefile
#
# Copyright (c) 2010, 2011, 2012 Jörgen Grahn
# All rights reserved.

SHELL=/bin/sh
INSTALLBASE=/usr/local
CXXFLAGS=-Wall -Wextra -pedantic -std=c++98 -g -O3 -Wold-style-cast
CPPFLAGS=-I..

.PHONY: all
all: libhttpd.a
all: httpd

.PHONY: install
install: httpd
install: httpd.1
	install -m755 httpd $(INSTALLBASE)/bin/
	install -m644 httpd.1 $(INSTALLBASE)/man/man1/

.PHONY: check checkv
check: tests
	./tests
checkv: tests
	valgrind -q ./tests

libhttpd.a: version.o
libhttpd.a: events.o
libhttpd.a: session.o
libhttpd.a: command.o
libhttpd.a: filter.o
libhttpd.a: deflate.o
#libhttpd.a: response.o
libhttpd.a: responsebuf.o
	$(AR) -r $@ $^

filter.o: CXXFLAGS+=-Wno-old-style-cast

httpd: httpd.o libhttpd.a
	$(CXX) -o $@ httpd.o -L. -lhttpd -L../sockutil -lsocket -lgdbm

#libtest.a: test/test_response.o
libtest.a: test/test_command.o
libtest.a: test/test_responsebuf.o
libtest.a: test/test_dbfile.o
	$(AR) -r $@ $^

test.cc: libtest.a
	testicle -o$@ $^

tests: test.o libhttpd.a libtest.a
	$(CXX) -o $@ test.o -L. -ltest -lhttpd -lgdbm

test/%.o: CPPFLAGS+=-I.

%.1.ps : %.1
	groff -man $< >$@

.PHONY: tags
tags: TAGS
TAGS:
	etags *.cc *.h

depend:
	makedepend -- $(CXXFLAGS) $(CPPFLAGS) -- -Y -I. *.cc test/*.cc

.PHONY: clean
clean:
	$(RM) httpd
	$(RM) *.o
	$(RM) *.ps
	$(RM) libhttpd.a
	$(RM) libtest.a
	$(RM) test/*.o
	$(RM) test.cc tests
	$(RM) Makefile.bak TAGS

love:
	@echo "not war?"

# DO NOT DELETE

command.o: command.h responsebuf.h session.h time.h textread.h requestqueue.h
command.o: response.h
deflate.o: deflate.h blob.h error.h
events.o: events.h session.h time.h textread.h requestqueue.h response.h
filter.o: filter.h blob.h deflate.h error.h
httpd.o: version.h events.h session.h time.h textread.h requestqueue.h
httpd.o: response.h
responsebuf.o: responsebuf.h
session.o: session.h time.h textread.h requestqueue.h response.h command.h
textread.o: textread.h
version.o: version.h
test/test_command.o: command.h
test/test_response.o: response.h
test/test_responsebuf.o: responsebuf.h
