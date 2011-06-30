# $Id: Makefile,v 1.19 2011-06-30 09:15:29 grahn Exp $
#
# Makefile
#
# Copyright (c) 2010, 2011 Jörgen Grahn
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
libgresabladet.a: session.o
libgresabladet.a: command.o
libgresabladet.a: response.o
libgresabladet.a: responsebuf.o
libgresabladet.a: dbfile.o
	$(AR) -r $@ $^

gresabladet: gresabladet.o libgresabladet.a
	$(CXX) -o $@ gresabladet.o -L. -lgresabladet -L../sockutil -lsocket -lgdbm

#libtest.a: test/test_response.o
libtest.a: test/test_responsebuf.o
libtest.a: test/test_dbfile.o
	$(AR) -r $@ $^

test.cc: libtest.a
	testicle -o$@ $^

tests: test.o libgresabladet.a libtest.a
	$(CXX) -o $@ test.o -L. -ltest -lgresabladet -lgdbm

test/%.o : test/%.cc
	$(CXX) -I. $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

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

command.o: command.h responsebuf.h response.h number.h msgid.h
dbfile.o: dbfile.h
gresabladet.o: version.h session.h ../sockutil/textread.h responsebuf.h
response.o: response.h number.h msgid.h
responsebuf.o: responsebuf.h
session.o: session.h ../sockutil/textread.h responsebuf.h command.h
version.o: version.h
test/test_dbfile.o: dbfile.h
test/test_response.o: response.h number.h msgid.h responsebuf.h
test/test_responsebuf.o: responsebuf.h
