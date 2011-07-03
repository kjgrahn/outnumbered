/*
 * $Id: test_command.cc,v 1.1 2011-07-03 11:37:10 grahn Exp $
 *
 * Copyright (C) 2011 Jörgen Grahn.
 * All rights reserved.
 */
#include <command.h>

#include <testicle.h>

namespace {

    void assert_parses(const std::string& s,
		       Command::Type type,
		       unsigned nargs)
    {}
}


namespace command {

    namespace broken {

    }

    namespace case {

    }

    void test_a()
    {
	assert_parses("ARTICLE\r\n", Command::ARTICLE, );
	assert_parses("ARTICLE 123\r\n", Command::ARTICLE, );
	assert_parses("ARTICLE 300256\r\n", Command::ARTICLE, );
	assert_parses("ARTICLE <45223423@example.com>\r\n", Command::ARTICLE, );
	assert_parses("ARTICLE a.message.id@no.angle.brackets\r\n", Command::ARTICLE, );
	assert_parses("ARTICLE <i.am.not.there@example.com>\r\n", Command::ARTICLE, );
    }

    void test_b()
    {
	assert_parses("BODY\r\n", Command::BODY, );
	assert_parses("BODY 300256\r\n", Command::BODY, );
	assert_parses("BODY <45223423@example.com>\r\n", Command::BODY, );
	assert_parses("BODY <i.am.not.there@example.com>\r\n", Command::BODY, );
    }

    void test_c()
    {
	assert_parses("CAPABILITIES\r\n", Command::CAPABILITIES, );
	assert_parses("CAPABILITIES AUTOUPDATE\r\n", Command::CAPABILITIES, );
    }

    void test_d()
    {
	assert_parses("DATE\r\n", Command::DATE, );
    }

    void test_g()
    {
	assert_parses("GROUP alt.crossposts\r\n", Command::GROUP, );
	assert_parses("GROUP archive.local\r\n", Command::GROUP, );
	assert_parses("GROUP binary.group\r\n", Command::GROUP, );
	assert_parses("GROUP example.currently.empty.newsgroup\r\n", Command::GROUP, );
	assert_parses("GROUP example.empty.newsgroup\r\n", Command::GROUP, );
	assert_parses("GROUP example.is.sob.bradner.or.barber\r\n", Command::GROUP, );
	assert_parses("GROUP misc.test\r\n", Command::GROUP, );
	assert_parses("GROUP secret.group\r\n", Command::GROUP, );
    }

    void test_h()
    {
	assert_parses("HDR Content-Type 3000234-3000238\r\n", Command::HDR, );
	assert_parses("HDR :lines 3000234-3000238\r\n", Command::HDR, );
	assert_parses("HDR subject\r\n", Command::HDR, );
	assert_parses("HDR subject 1-\r\n", Command::HDR, );
	assert_parses("HDR Subject 3000234-3000238\r\n", Command::HDR, );
	assert_parses("HDR subject 300256-\r\n", Command::HDR, );
	assert_parses("HDR subject <i.am.a.test.article@example.com>\r\n", Command::HDR, );
	assert_parses("HDR subject <i.am.not.there@example.com>\r\n", Command::HDR, );
	assert_parses("HEAD\r\n", Command::HEAD, );
	assert_parses("HEAD 300256\r\n", Command::HEAD, );
	assert_parses("HEAD <45223423@example.com>\r\n", Command::HEAD, );
	assert_parses("HEAD 53 54 55\r\n", Command::HEAD, );
	assert_parses("HEAD <i.am.not.there@example.com>\r\n", Command::HEAD, );
	assert_parses("HELP\r\n", Command::HELP, );
    }

    void test_i()
    {
	assert_parses("IHAVE <i.am.an.article.you.defer@example.com>\r\n", Command::IHAVE, );
	assert_parses("IHAVE <i.am.an.article.you.have@example.com>\r\n", Command::IHAVE, );
	assert_parses("IHAVE <i.am.an.article.you.will.want@example.com>\r\n", Command::IHAVE, );
    }

    void test_l()
    {
	assert_parses("LAST\r\n", Command::LAST, );
	assert_parses("LIST\r\n", Command::LIST, );
	assert_parses("LIST ACTIVE\r\n", Command::LIST, );
	assert_parses("LIST ACTIVE *.recovery\r\n", Command::LIST, );
	assert_parses("LIST ACTIVE.TIMES\r\n", Command::LIST, );
	assert_parses("LIST ACTIVE.TIMES tx.*\r\n", Command::LIST, );
	assert_parses("LIST ACTIVE u[ks].*\r\n", Command::LIST, );
	assert_parses("LIST DISTRIB.PATS\r\n", Command::LIST, );
	assert_parses("LISTGROUP\r\n", Command::LISTGROUP, );
	assert_parses("LISTGROUP example.empty.newsgroup\r\n", Command::LISTGROUP, );
	assert_parses("LISTGROUP misc.test\r\n", Command::LISTGROUP, );
	assert_parses("LISTGROUP misc.test 12345678-\r\n", Command::LISTGROUP, );
	assert_parses("LISTGROUP misc.test 3000238-3000248\r\n", Command::LISTGROUP, );
	assert_parses("LISTGROUP misc.test 9999-111\r\n", Command::LISTGROUP, );
	assert_parses("LIST HEADERS\r\n", Command::LIST, );
	assert_parses("LIST HEADERS MSGID\r\n", Command::LIST, );
	assert_parses("LIST HEADERS RANGE\r\n", Command::LIST, );
	assert_parses("LIST NEWSGROUPS\r\n", Command::LIST, );
	assert_parses("LIST OVERVIEW.FMT\r\n", Command::LIST, );
	assert_parses("LIST XTRA.DATA\r\n", Command::LIST, );
    }

    void test_m()
    {
	assert_parses("MAIL\r\n", Command::MAIL, );
	assert_parses("MODE POSTER\r\n", Command::MODE, );
	assert_parses("MODE READER\r\n", Command::MODE, );
    }

    void test_n()
    {
	assert_parses("NEWGROUPS 19990624 000000 GMT\r\n", Command::NEWGROUPS, );
	assert_parses("NEWNEWS alt.* 19990624 000000 GMT\r\n", Command::NEWNEWS, );
	assert_parses("NEWNEWS local.chat 20001231 235959 GMT\r\n", Command::NEWNEWS, );
	assert_parses("NEWNEWS local.chat 20010203 111933 GMT\r\n", Command::NEWNEWS, );
	assert_parses("NEWNEWS news.*,sci.* 19990624 000000 GMT\r\n", Command::NEWNEWS, );
	assert_parses("NEXT\r\n", Command::NEXT, );
    }

    void test_o()
    {
	assert_parses("OVER\r\n", Command::OVER, );
	assert_parses("OVER 3000234-3000240\r\n", Command::OVER, );
	assert_parses("OVER 3000444-3000222\r\n", Command::OVER, );
	assert_parses("OVER 300256\r\n", Command::OVER, );
	assert_parses("OVER <45223423@example.com>\r\n", Command::OVER, );
    }

    void test_p()
    {
	assert_parses("POST\r\n", Command::POST, );
    }

    void test_q()
    {
	assert_parses("QUIT\r\n", Command::QUIT, );
    }

    void test_s()
    {
	assert_parses("STAT\r\n", Command::STAT, );
	assert_parses("STAT 3002322\r\n", Command::STAT, );
	assert_parses("STAT 300256\r\n", Command::STAT, );
	assert_parses("STAT 444\r\n", Command::STAT, );
	assert_parses("STAT <45223423@example.com>\r\n", Command::STAT, );
	assert_parses("STAT <i.am.not.there@example.com>\r\n", Command::STAT, );
    }

    void test_x()
    {
	assert_parses("XENCRYPT\r\n", Command::XENCRYPT, );
	assert_parses("XENCRYPT RSA abcd=efg\r\n", Command::XENCRYPT, );
	assert_parses("XHOST binary.news.example.org\r\n", Command::XHOST, );
	assert_parses("XSECRET fred flintstone\r\n", Command::XSECRET, );
    }
}
