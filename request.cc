/* Copyright (c) 2013 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "request.h"

/*
   RFC 2616, section 5:

   A request is
   method SP Request-URI SP HTTP/n.n CRLF
   header CRLF
   header CRLF
   ...
   CRLF
   [body]

 */
