#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "ttf.h"
#include "ttfutil.h"

#ifdef MEMCHECK
#include <dmalloc.h>
#endif

/* 	$Id: disasm.c,v 1.1 2004/01/08 13:45:06 rahtz Exp $	 */

#ifndef lint
static char vcid[] = "$Id: disasm.c,v 1.1 2004/01/08 13:45:06 rahtz Exp $";
#endif /* lint */

void ttfPrintInstructions(FILE *fp, BYTE * ins)
{
   /* not implemented yet */ 
}

/* Pushing data onto the interpreter stack */
