#ifndef __TTF_TTC_H
#define __TTF_TTC_H

/* $Id: ttc.h,v 1.1 2004/01/08 13:45:06 rahtz Exp $ */

#include "ttf.h"

typedef struct
{
  ULONG TTCTag;
  Fixed version;
  ULONG DirCount;
  ULONG *offset;                /* length = DirCount */
  TTFontPtr font;
}
TTCHeader, *TTCHeaderPtr;

TTCHeaderPtr ttfLoadTTCHeader(char *filename);
void ttfLoadTTCFont(TTCHeaderPtr ttc, FILE *fp);
void ttfFreeTTCFont(TTCHeaderPtr ttc);

#endif /* __TTF_TTC_H */


/* end of ttc.h */
