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
all: liboutnumbered.a
all: outnumbered

.PHONY: install
install: outnumbered
install: outnumbered.1
	install -m755 outnumbered $(INSTALLBASE)/bin/
	install -m644 outnumbered.1 $(INSTALLBASE)/man/man1/

.PHONY: check checkv
check: tests
	./tests
checkv: tests
	valgrind -q ./tests

liboutnumbered.a: version.o
liboutnumbered.a: events.o
liboutnumbered.a: times.o
liboutnumbered.a: session.o
liboutnumbered.a: textread.o
liboutnumbered.a: requestqueue.o
liboutnumbered.a: command.o
liboutnumbered.a: filter.o
liboutnumbered.a: response.o
liboutnumbered.a: responsebuf.o
	$(AR) -r $@ $^

filter.o: CXXFLAGS+=-Wno-old-style-cast

outnumbered: httpd.o liboutnumbered.a
	$(CXX) -o $@ httpd.o -L. -loutnumbered -lrt

#libtest.a: test/test_response.o
libtest.a: test/request.o
libtest.a: test/test_command.o
libtest.a: test/test_responsebuf.o
libtest.a: test/test_dbfile.o
	$(AR) -r $@ $^

test.cc: libtest.a
	testicle -o$@ $^

tests: test.o liboutnumbered.a libtest.a
	$(CXX) -o $@ test.o -L. -ltest -loutnumbered -lgdbm

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
	$(RM) outnumbered
	$(RM) *.o
	$(RM) *.ps
	$(RM) liboutnumbered.a
	$(RM) libtest.a
	$(RM) test/*.o
	$(RM) test.cc tests
	$(RM) Makefile.bak TAGS

love:
	@echo "not war?"

# DO NOT DELETE

command.o: command.h responsebuf.h session.h times.h textread.h
command.o: requestqueue.h response.h
deflate.o: deflate.h blob.h error.h
events.o: events.h session.h times.h textread.h requestqueue.h response.h
filter.o: filter.h blob.h deflate.h error.h
httpd.o: version.h events.h session.h times.h textread.h requestqueue.h
httpd.o: response.h
requestqueue.o: requestqueue.h
response.o: response.h
responsebuf.o: responsebuf.h
session.o: session.h times.h textread.h requestqueue.h response.h command.h
textread.o: textread.h
times.o: times.h
version.o: version.h
test/request.o: ./request.h
test/test_command.o: command.h
test/test_response.o: response.h
test/test_responsebuf.o: responsebuf.h
