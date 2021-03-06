/*

  					W3C Sample Code Library libwww Identity Socket Read


!
  Identity Socket Read Method
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This function is an "identity" application protocol in that it reads
data from the transport and passes it down stream without doing anything
with it. It simply reads data and is a wrapper around a registered
transport mechanism for reading from a socket.
It provides a callback function for the event loop
so that a socket can be loaded using non-blocking I/O. The function requires
an open socket. It will typically be used in server applications or
in a client application that can read directly from stdin.

This module is implemented by HTSocket.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTSOCKET_H
#define HTSOCKET_H

#include "HTProt.h"

extern HTProtCallback HTLoadSocket;

/*
*/


/*
*/
#endif

/*

  

  @(#) $Id: HTSocket.h,v 1.1 2004/01/08 13:43:09 rahtz Exp $

*/
