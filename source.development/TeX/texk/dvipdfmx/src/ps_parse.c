/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/ps_parse.c,v 1.1 2004/01/08 13:44:15 rahtz Exp $
    
    This is dvipdfmx, an eXtended version of dvipdfm by Mark A. Wicks.

    Copyright (C) 2002 by Jin-Hwan Cho and Shunsaku Hirata,
    the dvipdfmx project team <dvipdfmx@project.ktug.or.kr>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

/*
  Simple PostScript parser:
    Does not known about PostScript operators. Just split tokens.
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "system.h"
#include "ps_parse.h"

static unsigned char verbose = 0;

void ps_parse_set_verbose (void)
{
  if (verbose < 255) verbose++;
}

static void skip_line (FILE *stream)
{
  char ch;
  while ((ch = fgetc(stream)) != EOF && ch != '\n' && ch != '\r') {
  }
}

/* read a line */
int ps_parse_read_line (FILE *stream, char *buffer, int bufflen)
{
  int i = 0;
  char ch;

  if (buffer == NULL) {
    fprintf(stderr, "no space allocated for input buffer\n");
    return PS_PARSE_BUFF_ERROR;
  }

  while ((ch = fgetc(stream)) != EOF && ch != '\n' && ch != '\r') {
    if (i == bufflen)
      return PS_PARSE_BUFF_ERROR; /* buffer error */;
    buffer[i++] = ch;
  }
  if (ch == EOF)
    return PS_PARSE_INVALID;

  buffer[i] = 0;

  return PS_PARSE_OK;
}

int ps_parse_get_token (FILE *stream, int *type,  char *buffer, int bufflen)
{
  int i = 0;
  char ch;
  int unbalanced = 0, escape = 0; /* for STRING type */
  int have_digit = 0, have_exp = 0; /* for INTEGER/REAL type */
  
  if (type == NULL || buffer == NULL || bufflen < 1) {
    fprintf(stderr, "no space allocated for input buffer");
    return PS_PARSE_BUFF_ERROR;
  }

  /* skip whitespaces, comments */
  while ((ch = fgetc(stream)) != EOF) {
    if (ch == '%') {
      skip_line (stream);
    } else if (!isspace(ch)) {
      break;
    }
  }
  if (ch == EOF) {
    *type = PS_UNKNOWN;
    return PS_PARSE_END;
  }

  switch (ch) {
  case '/':
    *type = PS_NAME;
    break;
  case '(':
    *type = PS_STRING;
    break;
  case '<': /* hex or a85 or dict */
    ch = fgetc(stream);
    if (ch == '<') {
      *type = PS_BEGIN_DICT;
      return PS_PARSE_OK;
    } else if (ch == '~') {
      *type = PS_STRING_A85;
    } else if (isxdigit(ch)) {
      *type = PS_STRING_AHX;
      buffer[i++] = ch;
    } else {
      return PS_PARSE_INVALID;
    }
    break;
  case '[':
    *type = PS_BEGIN_ARRAY;
    return PS_PARSE_OK;
    break;
  case ']':
    *type = PS_END_ARRAY;
    return PS_PARSE_OK;
    break;
  case '{':
    *type = PS_BEGIN_PROC;
    return PS_PARSE_OK;
    break;
  case '}':
    *type = PS_END_PROC;
    return PS_PARSE_OK;
    break;
  case '>':
    if (fgetc(stream) == '>') {
      *type = PS_END_DICT;
      return PS_PARSE_OK;
    } else {
      if (verbose)
	fprintf(stderr, "unexpected character `>'\n");
      *type = PS_UNKNOWN;
      return PS_PARSE_INVALID;
    }
    break;
  case ')':
    if (verbose)
      fprintf(stderr, "unexpected character `%c'\n", ch);
    *type = PS_UNKNOWN;
    return PS_PARSE_INVALID;
    break;
  default:
    if (isdigit(ch)) {
      have_digit = 1;
      *type = PS_INTEGER; /* actually undecided (maybe TOKEN/REAL/RADIX) */
    } else if (ch == '-' || ch == '+') {
      *type = PS_INTEGER;
    } else if (ch == '.') {
      *type = PS_REAL;
    } else {
      *type = PS_TOKEN;
    }
    buffer[i++] = ch;
    break;
  }

#define isdelim(ch) \
ch == '(' || ch == '<' || ch == '[' || ch == ']' \
|| ch == '{' || ch == '}' || ch == '/' || ch == '%'

  while ((ch = fgetc(stream)) != EOF) {
    if (*type == PS_TOKEN || *type == PS_NAME) {
      if (isspace(ch)) {
	break; /* not strict */
      } else if (isdelim(ch)) {
	ungetc(ch, stream);
	break;
      }
    } else if (*type == PS_INTEGER) {
      if (isspace(ch)) {
	break;
      } else if (isdelim(ch)) {
	ungetc(ch, stream);
	break;
      } else if (ch == '#') {
	/* check base (2-36) here */
	*type = PS_RADIX;
      } else if (ch == '.' && !have_exp) {
	*type = PS_REAL;
      } else if (ch == '+' || ch == '-') {
	if (i < 1 || (buffer[i-1] != 'e' && buffer[i-1] != 'E')) {
	  *type = PS_TOKEN;
	}
      } else if ((ch == 'e' || ch == 'E') && have_digit ) {
	have_exp = 1;
      } else if (isdigit(ch)) {
	have_digit = 1;
      } else {
	*type = PS_TOKEN; /* not a number */
      }
    } else if (*type == PS_REAL) {
      if (isspace(ch)) {
	break;
      } else if (isdelim(ch)) {
	ungetc(ch, stream);
	break;
      } else if (ch == '+' || ch == '-') {
	if (i < 1 || (buffer[i-1] != 'e' && buffer[i-1] != 'E')) {
	  *type = PS_TOKEN;
	}
      } else if ((ch == 'e' || ch == 'E') && have_digit ) {
	have_exp = 1;
      } else if (isdigit(ch)) {
	have_digit = 1;
      } else {
	*type = PS_TOKEN; /* not a number */
      }
    } else if (*type == PS_RADIX) {
      if (isspace(ch)) {
	break;
      } else if (isdelim(ch)) {
	ungetc(ch, stream);
	break;
      } else if (!(isalpha(ch) || isdigit(ch))) {
	*type = PS_TOKEN;
      }
      /* string is somewhat special */
    } else if (*type == PS_STRING) {
      if (escape) {
	escape = 0;
      } else {
	if (ch == '(') {
	  unbalanced++;
	} else if (ch == ')') {
	  if (!unbalanced)
	    break;
	  unbalanced--;
	} else if (ch == '\\') {
	  escape = 1;
	}
      }
    } else if (*type == PS_STRING_AHX) {
      if (ch == '>') {
	break;
      } else if (isspace(ch)) {
	continue; /* next */
      } else if (!isxdigit(ch)) {
	if (verbose)
	  fprintf(stderr, "non hex digit appears in <...>\n");
	*type = PS_UNKNOWN;
	return PS_PARSE_INVALID;
      }
      /* not strictly checking string */
    } else if (*type == PS_STRING_A85) {
      if (ch == '~' && fgetc(stream) == '>') {
	break;
      } else if (ch < '!' || (ch > 'u' && ch != 'z')) {
	*type = PS_UNKNOWN;
	return PS_PARSE_INVALID;
      }
    }

    if (i >= bufflen) { /* error */
      *type = PS_UNKNOWN;
      return PS_PARSE_BUFF_ERROR;
    }
    buffer[i++] = ch;
  }
  buffer[i] = 0;

  if (ch == EOF) {
    if (i == 0) {
      return PS_PARSE_END;
    } else {
      *type = PS_UNKNOWN;
      return PS_PARSE_INVALID;
    }
  }

  /*
  if (*type == PS_RADIX && check_radix(buffer) < 0)
    *type = TOKEN;
  */

  if ((*type == PS_REAL || *type == PS_INTEGER) && !have_digit)
    *type = PS_TOKEN;

  return PS_PARSE_OK;
}
