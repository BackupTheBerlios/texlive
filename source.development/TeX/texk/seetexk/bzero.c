#ifndef lint
static char rcsid[] = "$Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/seetexk/bzero.c,v 1.1 2004/01/08 13:44:42 rahtz Exp $";
#endif

/*
 * Sample bzero() routine.
 * This should be rewritten to be as fast as possible for your
 * machine.
 */
bzero(addr, count)
	register char *addr;
	register int count;
{

	while (--count >= 0)
		*addr++ = 0;
}
