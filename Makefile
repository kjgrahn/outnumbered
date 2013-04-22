#
# Makefile
#
# Copyright (c) 2010--2013 Jörgen Grahn
# All rights reserved.

SHELL=/bin/sh
INSTALLBASE=/usr/local
CXXFLAGS=-Wall -Wextra -pedantic -std=c++98 -g -Os -Wold-style-cast
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
	valgrind -q ./tests -v

liboutnumbered.a: version.o
liboutnumbered.a: log.o
liboutnumbered.a: server.o
liboutnumbered.a: times.o
liboutnumbered.a: session.o
liboutnumbered.a: sessionhistory.o
liboutnumbered.a: textread.o
liboutnumbered.a: requestqueue.o
liboutnumbered.a: request.o
liboutnumbered.a: names.o
liboutnumbered.a: filter.o
liboutnumbered.a: deflate.o
liboutnumbered.a: response.o
liboutnumbered.a: input.o
	$(AR) -r $@ $^

filter.o: CXXFLAGS+=-Wno-old-style-cast
deflate.o: CXXFLAGS+=-Wno-old-style-cast
httpd.o: CXXFLAGS+=-Wno-old-style-cast

outnumbered: httpd.o liboutnumbered.a
	$(CXX) -o $@ httpd.o -L. -loutnumbered -lrt -lz

magic: magic.o liboutnumbered.a
	$(CXX) -o $@ magic.o -L. -loutnumbered -lmagic

#libtest.a: test/test_response.o
libtest.a: test/test_request.o
libtest.a: test/test_filter.o
libtest.a: test/test_log.o
libtest.a: test/pipe.o
libtest.a: test/test_deflate.o
	$(AR) -r $@ $^

test.cc: libtest.a
	testicle -o$@ $^

tests: test.o liboutnumbered.a libtest.a
	$(CXX) -o $@ test.o -L. -ltest -loutnumbered -lz

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
	$(RM) outnumbered magic
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

deflate.o: deflate.h blob.h error.h
filter.o: filter.h blob.h deflate.h error.h
httpd.o: version.h error.h server.h session.h times.h textread.h
httpd.o: requestqueue.h response.h filter.h blob.h deflate.h input.h
input.o: input.h
log.o: log.h
magic.o: version.h
names.o: names.h request.h blob.h
request.o: request.h blob.h names.h
requestqueue.o: requestqueue.h
response.o: response.h filter.h blob.h deflate.h input.h
responsebuf.o: responsebuf.h
server.o: server.h session.h times.h textread.h requestqueue.h response.h
server.o: filter.h blob.h deflate.h input.h error.h
session.o: session.h times.h textread.h requestqueue.h response.h filter.h
session.o: blob.h deflate.h input.h log.h
sessionhistory.o: session.h times.h textread.h requestqueue.h response.h
sessionhistory.o: filter.h blob.h deflate.h input.h
textread.o: textread.h
times.o: times.h
version.o: version.h
test/pipe.o: test/pipe.h blob.h
test/test_deflate.o: deflate.h blob.h
test/test_filter.o: filter.h blob.h deflate.h test/pipe.h
test/test_log.o: log.h
test/test_request.o: request.h blob.h
test/test_response.o: response.h filter.h blob.h deflate.h input.h
test/test_responsebuf.o: responsebuf.h
