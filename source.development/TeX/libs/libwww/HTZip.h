/*

  
  					W3C Sample Code Library libwww ZLib Compress and Decompress Stream


!
  ZLib Compress and Decompress Stream
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module provides an interface to the zlib compress and decompress functions. It can be hooked inas as content encoding coder/decoder in libwww which allows for on-the-fly encoding/decoding.

This module is implemented by HTZip.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTZIP_H
#define HTZIP_H

#include "HTFormat.h"

/*
*/

#ifdef HT_ZLIB
extern HTCoder HTZLib_inflate;
#endif

/*

End of definition module
*/

#endif /* HTZIP_H */

/*

  

  @(#) $Id: HTZip.h,v 1.1 2004/01/08 13:43:10 rahtz Exp $

*/
