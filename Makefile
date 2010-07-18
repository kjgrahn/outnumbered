# $Id: Makefile,v 1.1 2010-07-18 09:46:23 grahn Exp $
#
# Makefile
#
# Copyright (c) 2010 Jörgen Grahn
# All rights reserved.

SHELL=/bin/sh
INSTALLBASE=/usr/local
CXXFLAGS=-Wall -Wextra -pedantic -std=c++98 -g -O2

.PHONY: all
all: libgresabladet.a
all: gresabladet

.PHONY: install
install: gresabladet
install: gresabladet.1
	install -m755 gresabladet $(INSTALLBASE)/bin/
	install -m644 gresabladet.1 $(INSTALLBASE)/man/man1/

.PHONY: check checkv
check: tests
	./tests
checkv: tests
	valgrind -q ./tests

libgresabladet.a: version.o
	$(AR) -r $@ $^

gresabladet: gresabladet.o libprefer.a
	$(CXX) -o $@ gresabladet.o -L. -lgresabladet

libtest.a: 
	$(AR) -r $@ $^

test.cc: libtest.a
	testicle -o$@ $^

tests: test.o libgresabladet.a libtest.a
	$(CXX) -o $@ test.o -L. -ltest -lgresabladet

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

