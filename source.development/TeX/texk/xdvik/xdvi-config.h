/* xdvi-config.h: master configuration file, included first by all
   compilable source files (not headers).  */

#ifndef CONFIG_H
#define CONFIG_H

#ifndef KPATHSEA
#define KPATHSEA 1
#endif

#ifndef HAVE_VPRINTF
#ifdef HAVE_DOPRNT
#define	vfprintf(stream, message, args)	_doprnt(message, args, stream)
/* If we have neither, should fall back to fprintf with fixed args.  */
#endif
#endif

/* Some xdvi options we want by default.  */
#define USE_PK
#define USE_GF
#define MAKEPK

#ifndef NOSELFILE
#define SELFILE
#endif
#ifndef NOGRID
#define GRID
#endif
#ifndef NOTEXXET
#define TEXXET
#endif

/* On the other hand, NOTOOL means we lose practically everything.  */
#ifdef NOTOOL
#undef SELFILE
#undef BUTTONS
#undef GRID
#undef HTEX
#endif /* NOTOOL */

/* xdvi's definitions.  */
#include "xdvi.h"
#include "c-auto.h"

#define register

#endif /* not CONFIG_H */
