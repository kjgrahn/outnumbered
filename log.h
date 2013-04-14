/* -*- c++ -*-
 *
 * Copyright (c) 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OUTN_LOG_H_
#define OUTN_LOG_H_

#include <iostream>
#include <streambuf>
#include <vector>

#include <syslog.h>


/**
 * Crude ostream interface to syslog(3).  We want an efficient way of
 * saying something like:
 *
 *    LOG_WARNING << "session " << s << " closed unexpectedly";
 *
 * and have it translate to one syslog message.
 *
 * Class Syslog implements the ostream, a fixed, limited-size stream buffer,
 * and the syslogging.  The Log<Prio> template makes the syntax acceptable.
 * Overlong messages will simply be truncated.
 */
class Syslog : private std::basic_streambuf<char> {
public:
    Syslog();
    ~Syslog();
    std::ostream& ostream() { return os; }
    void flush(int prio);

private:
    std::vector<char_type> v;
    std::ostream os;
};


/**
 * Since the syslog is naturally process-global and reentrancy is
 * irrelevant here, we might as well provide a global Syslog object.
 */
extern Syslog log;


template <int Prio>
struct Log {
    explicit Log(Syslog& syslog) : syslog(syslog) {}
    ~Log() { syslog.flush(Prio); }

    template <class T>
    std::ostream& operator<< (const T& t) {
	return syslog.ostream() << t;
    }

private:
    Syslog& syslog;

    Log();
    Log(const Log&);
    Log& operator= (const Log&);
};

typedef Log<LOG_EMERG> Emerg;
typedef Log<LOG_ALERT> Alert;
typedef Log<LOG_CRIT> Crit;
typedef Log<LOG_ERR> Err;
typedef Log<LOG_WARNING> Warning;
typedef Log<LOG_NOTICE> Notice;
typedef Log<LOG_INFO> Info;
typedef Log<LOG_DEBUG> Debug;

#endif
