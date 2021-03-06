/* Provide definitions for missing or incorrect libc functions. */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_STRDUP
char *
strdup(const char *s)
{
  char *t;
  int l;
  if (!s)
    return 0;
  l = strlen(s) + 1;
  t = (char *)malloc(l);
  if (!t)
    return 0;
  memcpy(t, s, l);
  return t;
}
#endif

#ifndef HAVE_STRERROR
/* David Mazieres <dm@lcs.mit.edu> assures me that this definition works. */
char *
strerror(int errno)
{
  extern int sys_nerr;
  extern char *sys_errlist[];
  if (errno < 0 || errno >= sys_nerr)
    return (char *)"bad error number";
  else
    return sys_errlist[errno];
}
#endif

#ifdef BROKEN_STRTOD
/* On NeXTSTEP, Melissa O'Neill <oneill@cs.sfu.ca> reports that strtod
   consumes whitespace after its argument, which makes mminstance (among other
   programs) not work. This wrapper gets rid of that whitespace again.
   (Originally, we suspected strtol too, but it seems to work, at least in
   NeXTSTEP 3.3 patch 2.) */

double
good_strtod(const char *nptr, char **endptr)
{
  double d = strtod(nptr, endptr);
  if (endptr)
    while (*endptr > nptr && isspace((*endptr)[-1]))
      (*endptr)--;
  return d;
}
#endif

#ifdef __cplusplus
}
#endif
