# $Id: Makefile,v 1.23 2011-07-03 19:17:25 grahn Exp $
#
# Makefile
#
# Copyright (c) 2010, 2011, 2012 Jörgen Grahn
# All rights reserved.

SHELL=/bin/sh
INSTALLBASE=/usr/local
CXXFLAGS=-Wall -Wextra -pedantic -std=c++98 -g -O3
CPPFLAGS=-I..

.PHONY: all
all: libgresabladet.a
all: gresabladet

.PHONY: install
install: gresabladet
install: gresabladet.1
	install -m755 gresabladet $(INSTALLBASE)/bin/
	install -m644 gresabladet.1 $(INSTALLBASE)/man/man1/
	install -m644 gresabladet.5 $(INSTALLBASE)/man/man5/

.PHONY: check checkv
check: tests
	./tests
checkv: tests
	valgrind -q ./tests

libgresabladet.a: version.o
libgresabladet.a: events.o
libgresabladet.a: session.o
libgresabladet.a: command.o
#libgresabladet.a: response.o
libgresabladet.a: responsebuf.o
	$(AR) -r $@ $^

gresabladet: gresabladet.o libgresabladet.a
	$(CXX) -o $@ gresabladet.o -L. -lgresabladet -L../sockutil -lsocket -lgdbm

#libtest.a: test/test_response.o
libtest.a: test/test_command.o
libtest.a: test/test_responsebuf.o
libtest.a: test/test_dbfile.o
	$(AR) -r $@ $^

test.cc: libtest.a
	testicle -o$@ $^

tests: test.o libgresabladet.a libtest.a
	$(CXX) -o $@ test.o -L. -ltest -lgresabladet -lgdbm

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
	$(RM) gresabladet
	$(RM) *.o
	$(RM) *.ps
	$(RM) libgresabladet.a
	$(RM) libtest.a
	$(RM) test/*.o
	$(RM) test.cc tests
	$(RM) Makefile.bak TAGS

love:
	@echo "not war?"

# DO NOT DELETE

command.o: command.h responsebuf.h session.h time.h textread.h requestqueue.h
command.o: response.h
events.o: events.h session.h time.h textread.h requestqueue.h response.h
gresabladet.o: version.h events.h session.h time.h textread.h requestqueue.h
gresabladet.o: response.h
responsebuf.o: responsebuf.h
session.o: session.h time.h textread.h requestqueue.h response.h command.h
textread.o: textread.h
version.o: version.h
test/test_command.o: command.h
test/test_response.o: response.h
test/test_responsebuf.o: responsebuf.h
