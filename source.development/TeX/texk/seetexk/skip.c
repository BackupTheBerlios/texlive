/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/seetexk/skip.c,v 1.1 2004/01/08 13:44:44 rahtz Exp $";
#endif

#include <stdio.h>
#include "types.h"
#include "fio.h"

/*
 * Skip a font definition.  The font number has already been consumed.
 */
SkipFontDef(fp)
	register FILE *fp;
{
	register int i;

	(void) GetLong(fp);	/* checksum */
	(void) GetLong(fp);	/* mag */
	(void) GetLong(fp);	/* designsize */
	i = UnSign8(GetByte(fp)) + UnSign8(GetByte(fp));
	while (--i >= 0)
		(void) GetByte(fp);
}
