#ifndef lint
static char rcsid[] = "$Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/seetexk/bcopy.c,v 1.1 2004/01/08 13:44:42 rahtz Exp $";
#endif

/*
 * Sample bcopy() routine.
 * This should be rewritten to be as fast as possible for your
 * machine.
 */
void bcopy(from, to, count)
	register char *from, *to;
	register int count;
{

	if (from == to)
		return;
	if (from > to) {
		while (--count >= 0)
			*to++ = *from++;
	} else {
		from += count;
		to += count;
		while (--count >= 0)
			*--to = *--from;
	}
}
