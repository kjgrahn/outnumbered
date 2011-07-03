/*
 * $Id: test_command.cc,v 1.4 2011-07-03 19:40:17 grahn Exp $
 *
 * Copyright (C) 2011 Jörgen Grahn.
 * All rights reserved.
 */
#include <command.h>
#include <cstring>

#include <testicle.h>

namespace {

    void assert_parses(const std::string& s,
		       const Command::Type type,
		       const char* next = "")
    {
	const char* p = s.data();
	const char* q = p + s.size();
	Command::Type t = Command::parse(&p, q);
	testicle::assert_eq(t, type);
	testicle::assert_eq(p,
			    std::search(p, q,
					next, next+std::strlen(next)));
    }
}


namespace command {

    namespace broken {

    }

    namespace ignorecase {

	void test()
	{
	    assert_parses("list\r\n", Command::LIST, "");
	    assert_parses("List Active\r\n", Command::LIST_ACTIVE, "");
	    assert_parses("list active *.recovery\r\n", Command::LIST_ACTIVE, "*.rec");
	    assert_parses("liST acTive.tiMes\r\n", Command::LIST_ACTIVE_TIMES, "");
	    assert_parses("list ACTIVE.times tx.*\r\n", Command::LIST_ACTIVE_TIMES, "tx.");
	    assert_parses("LIST ACTIVE u[ks].*\r\n", Command::LIST_ACTIVE, "u[");
	}
    }

    void test_a()
    {
	assert_parses("ARTICLE\r\n", Command::ARTICLE, "");
	assert_parses("ARTICLE 123\r\n", Command::ARTICLE, "123");
	assert_parses("ARTICLE 300256\r\n", Command::ARTICLE, "30");
	assert_parses("ARTICLE <45223423@example.com>\r\n", Command::ARTICLE, "<45");
	assert_parses("ARTICLE a.message.id@no.angle.brackets\r\n", Command::ARTICLE, "a.");
	assert_parses("ARTICLE <i.am.not.there@example.com>\r\n", Command::ARTICLE, "<i");
    }

    void test_b()
    {
	assert_parses("BODY\r\n", Command::BODY, "");
	assert_parses("BODY 300256\r\n", Command::BODY, "30");
	assert_parses("BODY <45223423@example.com>\r\n", Command::BODY, "<45223423");
	assert_parses("BODY <i.am.not.there@example.com>\r\n", Command::BODY, "<i.am");
    }

    void test_c()
    {
	assert_parses("CAPABILITIES\r\n", Command::CAPABILITIES, "");
	assert_parses("CAPABILITIES AUTOUPDATE\r\n", Command::CAPABILITIES, "AUTOUP");
    }

    void test_d()
    {
	assert_parses("DATE\r\n", Command::DATE, "");
    }

    void test_g()
    {
	assert_parses("GROUP alt.crossposts\r\n", Command::GROUP, "alt");
	assert_parses("GROUP archive.local\r\n", Command::GROUP, "archive");
	assert_parses("GROUP binary.group\r\n", Command::GROUP, "binary");
	assert_parses("GROUP example.currently.empty.newsgroup\r\n", Command::GROUP, "ex");
	assert_parses("GROUP example.empty.newsgroup\r\n", Command::GROUP, "ex");
	assert_parses("GROUP example.is.sob.bradner.or.barber\r\n", Command::GROUP, "ex");
	assert_parses("GROUP misc.test\r\n", Command::GROUP, "mi");
	assert_parses("GROUP secret.group\r\n", Command::GROUP, "se");
    }

    void test_h()
    {
	assert_parses("HDR Content-Type 3000234-3000238\r\n", Command::HDR, "Content");
	assert_parses("HDR :lines 3000234-3000238\r\n", Command::HDR, ":lines");
	assert_parses("HDR subject\r\n", Command::HDR, "subj");
	assert_parses("HDR subject 1-\r\n", Command::HDR, "subj");
	assert_parses("HDR Subject 3000234-3000238\r\n", Command::HDR, "Subj");
	assert_parses("HDR subject 300256-\r\n", Command::HDR, "subj");
	assert_parses("HDR subject <i.am.a.test.article@example.com>\r\n", Command::HDR, "subj");
	assert_parses("HDR subject <i.am.not.there@example.com>\r\n", Command::HDR, "subj");
	assert_parses("HEAD\r\n", Command::HEAD, "");
	assert_parses("HEAD 300256\r\n", Command::HEAD, "30");
	assert_parses("HEAD <45223423@example.com>\r\n", Command::HEAD, "<45223423");
	assert_parses("HEAD 53 54 55\r\n", Command::HEAD, "53 54 55");
	assert_parses("HEAD <i.am.not.there@example.com>\r\n", Command::HEAD, "<i.am");
	assert_parses("HELP\r\n", Command::HELP, "");
    }

    void test_i()
    {
	assert_parses("IHAVE <i.am.an.article.you.defer@example.com>\r\n", Command::IHAVE, "<i.a");
	assert_parses("IHAVE <i.am.an.article.you.have@example.com>\r\n", Command::IHAVE, "<i.a");
	assert_parses("IHAVE <i.am.an.article.you.will.want@example.com>\r\n", Command::IHAVE, "<i.a");
    }

    void test_l()
    {
	assert_parses("LAST\r\n", Command::LAST, "");
	assert_parses("LIST\r\n", Command::LIST, "");
	assert_parses("LIST ACTIVE\r\n", Command::LIST_ACTIVE, "");
	assert_parses("LIST ACTIVE *.recovery\r\n", Command::LIST_ACTIVE, "*");
	assert_parses("LIST ACTIVE.TIMES\r\n", Command::LIST_ACTIVE_TIMES, "");
	assert_parses("LIST ACTIVE.TIMES tx.*\r\n", Command::LIST_ACTIVE_TIMES, "tx");
	assert_parses("LIST ACTIVE u[ks].*\r\n", Command::LIST_ACTIVE, "u");
	assert_parses("LIST DISTRIB.PATS\r\n", Command::LIST_DISTRIB_PATS, "");
	assert_parses("LISTGROUP\r\n", Command::LISTGROUP, "");
	assert_parses("LISTGROUP example.empty.newsgroup\r\n", Command::LISTGROUP, "exam");
	assert_parses("LISTGROUP misc.test\r\n", Command::LISTGROUP, "misc");
	assert_parses("LISTGROUP misc.test 12345678-\r\n", Command::LISTGROUP, "misc");
	assert_parses("LISTGROUP misc.test 3000238-3000248\r\n", Command::LISTGROUP, "misc");
	assert_parses("LISTGROUP misc.test 9999-111\r\n", Command::LISTGROUP, "misc");
	assert_parses("LIST HEADERS\r\n", Command::LIST_HEADERS, "");
	assert_parses("LIST HEADERS MSGID\r\n", Command::LIST_HEADERS, "MSGID\r\n");
	assert_parses("LIST HEADERS RANGE\r\n", Command::LIST_HEADERS, "RANGE");
	assert_parses("LIST NEWSGROUPS\r\n", Command::LIST_NEWSGROUPS, "");
	assert_parses("LIST OVERVIEW.FMT\r\n", Command::LIST_OVERVIEW_FMT, "");
	assert_parses("LIST XTRA.DATA\r\n", Command::UNKNOWN, "");
    }

    void test_m()
    {
	assert_parses("MAIL\r\n", Command::UNKNOWN, "");
	assert_parses("MODE POSTER\r\n", Command::UNKNOWN, "");
	assert_parses("MODE READER\r\n", Command::MODE_READER, "");
    }

    void test_n()
    {
	assert_parses("NEWGROUPS 19990624 000000 GMT\r\n", Command::NEWGROUPS, "1999");
	assert_parses("NEWNEWS alt.* 19990624 000000 GMT\r\n", Command::NEWNEWS, "alt.");
	assert_parses("NEWNEWS local.chat 20001231 235959 GMT\r\n", Command::NEWNEWS, "loca");
	assert_parses("NEWNEWS local.chat 20010203 111933 GMT\r\n", Command::NEWNEWS, "loca");
	assert_parses("NEWNEWS news.*,sci.* 19990624 000000 GMT\r\n", Command::NEWNEWS, "news");
	assert_parses("NEXT\r\n", Command::NEXT, "");
    }

    void test_o()
    {
	assert_parses("OVER\r\n", Command::OVER, "");
	assert_parses("OVER 3000234-3000240\r\n", Command::OVER, "3000");
	assert_parses("OVER 3000444-3000222\r\n", Command::OVER, "3000");
	assert_parses("OVER 300256\r\n", Command::OVER, "3002");
	assert_parses("OVER <45223423@example.com>\r\n", Command::OVER, "<452");
    }

    void test_p()
    {
	assert_parses("POST\r\n", Command::POST, "");
    }

    void test_q()
    {
	assert_parses("QUIT\r\n", Command::QUIT, "");
    }

    void test_s()
    {
	assert_parses("STAT\r\n", Command::STAT, "");
	assert_parses("STAT 3002322\r\n", Command::STAT, "3002");
	assert_parses("STAT 300256\r\n", Command::STAT, "3002");
	assert_parses("STAT 444\r\n", Command::STAT, "444");
	assert_parses("STAT <45223423@example.com>\r\n", Command::STAT, "<452");
	assert_parses("STAT <i.am.not.there@example.com>\r\n", Command::STAT, "<i.a");
    }

    void test_x()
    {
	assert_parses("XENCRYPT\r\n", Command::UNKNOWN, "");
	assert_parses("XENCRYPT RSA abcd=efg\r\n", Command::UNKNOWN, "RSA ");
	assert_parses("XHOST binary.news.example.org\r\n", Command::UNKNOWN, "bina");
	assert_parses("XSECRET fred flintstone\r\n", Command::UNKNOWN, "fred");
    }
}
