/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/pdfparse.c,v 1.1 2004/01/08 13:44:15 rahtz Exp $

    This is dvipdfmx, an eXtended version of dvipdfm by Mark A. Wicks.

    Copyright (C) 2002 by Jin-Hwan Cho and Shunsaku Hirata,
    the dvipdfmx project team <dvipdfmx@project.ktug.or.kr>
    
    Copyright (C) 1998, 1999 by Mark A. Wicks <mwicks@kettering.edu>

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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "system.h"
#include "mem.h"
#include "mfileio.h"
#include "numbers.h"
#include "dvi.h"
#include "pdfparse.h"
#include "pdfspecial.h"
#include "pdfobj.h"
#include "pdfdoc.h"
#include "pdfdev.h"

#ifndef WITHOUT_TOUNICODE
#include <stdarg.h>
#include "cmap.h"

static int tounicode_cmap = -1;

void set_tounicode_cmap (char *cmap_name)
{
  if (cmap_name) {
    tounicode_cmap = get_cmap(cmap_name);
    RELEASE (cmap_name);
  } else
    tounicode_cmap = -1;
}
#endif

static char *save;
#define DUMP_LIMIT 50
void dump (char *start, char *end)
{
  char *p = start;
  fprintf (stderr, "\nCurrent input buffer is -->");
  while (p < end && p < start+DUMP_LIMIT)
    fprintf (stderr, "%c", *(p++));
  if (p == start+DUMP_LIMIT)
    fprintf (stderr, "...\n");
  fprintf (stderr, "<--\n");
}

void skip_line (char **start, char *end)
{
  /* Note: PDF spec says that all platforms must end line with '\n'
     after a "stream" keyword */
  while (*start < end && **start != '\n' && **start != '\r')
    (*start)++;
  /* The carriage return (CR; \r; 0x0D) and line feed (LF; \n; 0x0A)
   * characters, also called newline characters, are treated as
   * end-of-line (EOL) markers. The combination of a carriage return
   * followed immediately by a line feed is treated as one EOL marker.
   */
  if (*start < end && **start == '\r')
    (*start)++;
  if (*start < end && **start == '\n')
    (*start)++;
}

void skip_white (char **start, char *end)
{
  /* The null (NUL; 0x00) character is a white-space character in PDF spec
   * but isspace(0x00) returns FALSE; on the other hand, the vertical tab
   * (VT; 0x0B) character is not a white-space character in PDF spec but
   * isspace(0x0B) returns TRUE. */
  while (*start < end && (isspace(**start) || **start == '%')) {
    if (**start == '%') 
      skip_line (start, end);
    else /* Skip the white char */
      (*start)++;
  }
}

int is_an_int (const char *s)
{
  if (!s || !*s) return 0;
  if (*s == '+' || *s == '-') s++;
  while (*s)
    if (!isdigit(*s++)) return 0;
  return 1;
}

int is_a_number (const char *s)
{
  if (!s || !*s) return 0;
  if (*s == '+' || *s == '-') s++;
  while (*s)
    if (!isdigit(*s++)) break;
  if (!*s) return 1;
  if (*(s-1) != '.') return 0;
  while (*s)
    if (!isdigit(*s++)) return 0;
  return 1;
}

static char *parsed_string (char *end)
{
  register char *result = NULL;
  register int len = end - save;
  if (len > 0) {
    result = NEW (len+1, char);
    memcpy(result, save, len);
    result[len] = 0;
  }
  return result;
}

char *parse_number (char **start, char *end)
{
  skip_white(start, end); save = *start;

  if (*start < end && (**start == '+' || **start == '-'))
    (*start)++;
  while (*start < end && isdigit(**start))
    (*start)++;
  if (*start < end && **start == '.') {
    (*start)++;
    while (*start < end && isdigit(**start))
      (*start)++;
  }
  return parsed_string(*start);
}

char *parse_unsigned (char **start, char *end)
{
  skip_white(start, end); save = *start;

  while (*start < end && isdigit(**start))
    (*start)++;
  return parsed_string(*start);
}

static char *parse_gen_ident (char **start, char *end, char *valid_chars)
{
  save = *start;
  while (*start < end && strchr(valid_chars, **start))
    (*start)++;
  return parsed_string(*start);
}

char *parse_ident (char **start, char *end)
{
  static char *valid_chars = "!\"#$&'*+,-.0123456789:;=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwxyz|~";
  return parse_gen_ident(start, end, valid_chars);
}

char *parse_val_ident (char **start, char *end)
{
  static char *valid_chars = "!\"#$&'*+,-./0123456789:;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwxyz|~";
  return parse_gen_ident(start, end, valid_chars);
}

char *parse_c_ident (char **start, char *end)
{
  static char *valid_chars = "0123456789@ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";
  return parse_gen_ident(start, end, valid_chars);
}

char *parse_opt_ident (char **start, char *end)
{
  if (*start < end && **start == '@') {
    (*start)++;
    return parse_ident(start, end);
  }
  return NULL;
}

static pdf_obj *parse_pdf_number (char **start, char *end)
{
  register pdf_obj *result;
  register char *number;

  skip_white(start, end); save = *start;

  if ((number = parse_number(start, end))) {
    result = pdf_new_number(atof(number));
    RELEASE (number);
    return result;
  }
  fprintf(stderr, "\nCould not find a numeric object.\n");
  dump(*start = save, end);
  return NULL;
}

pdf_obj *parse_pdf_name (char **start, char *end)
{
  register pdf_obj *result;
  register char *name;

  skip_white(start, end); save = *start;

  if (*start < end && **start == '/') {
    (*start)++;
    if ((name = parse_ident(start, end))) {
      result = pdf_new_name(name);
      RELEASE (name);
      return result;
    }
  }
  fprintf(stderr, "\nCould not find a name object.\n");
  dump(*start = save, end);
  return NULL;
}

pdf_obj *parse_pdf_boolean (char **start, char *end)
{
  register int len;

  skip_white (start, end); save = *start;

  if ((len = end - *start) >= 4 && !strncmp(*start, "true", 4)) {
    *start += 4;
    return pdf_new_boolean(1);
  }
  if (len >= 5 && !strncmp(*start, "false", 5)) {
    *start += 5;
    return pdf_new_boolean(0);
  }
  fprintf(stderr, "\nCould not find a boolean object.\n");
  dump(*start = save, end);
  return NULL;
}

pdf_obj *parse_pdf_null (char **start, char *end)
{
  register int len;

  skip_white (start, end); save = *start;

  if ((len = end - *start) >= 4 && !strncmp(*start, "null", 4)) {
    *start += 4;
    return pdf_new_null();
  }
  fprintf(stderr, "\nCould not find a null object.\n");
  dump(*start = save, end);
  return NULL;
}

static int parse_escape_char (char **start, char *end,
                              const char *escape_str, unsigned char *ch)
{
  if (**start != '\\') return 0;

  switch (*(++(*start))) {
  case 'n':
    *ch = '\n'; break;
  case 'r':
    *ch = '\r'; break;
  case 't':
    *ch = '\t'; break;
  case 'b':
    *ch = '\b'; break;
  case 'f':
    *ch = '\f'; break;
  default:
    if (strchr(escape_str, **start)) {
      *ch = **start; break;
    }
    if (isdigit(**start)) {
      *ch = *((*start)++) - '0';
      if (isdigit(**start)) {
        *ch = (*ch << 3) + (*((*start)++) - '0');
        if (isdigit(**start))
          *ch = (*ch << 3) + (*((*start)++) - '0');
      }
      return 1;
    }
    return 0;
  }
  (*start)++;
  return 1;
}

pdf_obj *parse_pdf_string (char **start, char *end)
{
  register pdf_obj *result;
  register unsigned char *string;
  register int balance = 0, len = 0;

  skip_white(start, end); save = *start;

  if (*start < end && **start == '(') {
    (*start)++;
    string = NEW (end - *start, unsigned char);
    while (*start < end && (**start != ')' || balance > 0))
      if (**start == '\\' &&
          parse_escape_char(start, end, "\\()", &(string[len]))) len++;
      else {
        if (**start == '(') balance += 1;
        if (**start == ')') balance -= 1;
        string[len++] = *((*start)++);
      }
    if (*start < end) {
      (*start)++;
      result = pdf_new_string(string, len);
      RELEASE (string);
      return result;
    }
  }
  fprintf(stderr, "\nCould not find a literal string object.\n");
  dump(*start = save, end);
  return NULL;
}

static unsigned char xtod (char ch)
{
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  if (ch >= 'A' && ch <= 'F')
    return (ch - 'A') + 10;
  if (ch >= 'a' && ch <= 'f')
    return (ch - 'a') + 10;
  return 0;
}

static unsigned char *parse_hexadecimal (char **start, char *end, int *len)
{
  register unsigned char *string;

  save = (*start)++; skip_white(start, end); *len = 0;
  string = NEW ((end - *start) / 2 + 1, unsigned char);
  while (*start < end && **start != '>') {
    string[*len] = xtod(*((*start)++)) << 4;
    if (*start < end && **start != '>')
      string[*len] += xtod(*((*start)++));
    (*len)++;
    skip_white(start, end);
  }
  if (*start < end) {
    (*start)++;
    return string;
  }
  fprintf(stderr, "\nHexadecimal string object ended prematurely.\n");
  dump(*start = save, end);
  return NULL;
}

pdf_obj *parse_pdf_hex_string (char **start, char *end)
{
  register pdf_obj *result;
  register unsigned char *string;
  int len;

  skip_white(start, end); save = *start;

  if (*start < end && **start == '<' &&
      (string = parse_hexadecimal(start, end, &len))) {
    result = pdf_new_stringx(string, len);
    RELEASE (string);
    return result;
  }
  fprintf(stderr, "\nCould not find a hexadecimal string object.\n");
  dump(*start = save, end);
  return NULL;
}

#ifndef WITHOUT_TOUNICODE
static pdf_obj *parse_pdf_string_tounicode (char **start, char *end)
{
  register pdf_obj *result;
  register unsigned char *ustring;
  register unsigned short code;
  register int balance = 0, ulen = 0;
  unsigned char ch;

  skip_white(start, end); save = *start;

  if (*start < end && **start == '(') {
    /* Call parse_pdf_string() if the string is already Unicode. */
    if (end - *start >= 9 && !strncmp(*start, "(\\376\\377", 9))
      return parse_pdf_string (start, end);
    (*start)++;
    ustring = NEW ((end - *start) * 2, unsigned char);
    ustring[ulen++] = 0xFE; ustring[ulen++] = 0xFF;
    while (*start < end && (**start != ')' || balance > 0))
      if (**start == '\\' && parse_escape_char(start, end, "\\()", &ch)) {
        ustring[ulen++] = 0;
        ustring[ulen++] = ch;
      } else if (**start == '(' || **start == ')') {
        if (**start == '(') balance++;
        else balance--;
        ustring[ulen++] = 0;
        ustring[ulen++] = *((*start)++);
      } else {
        if ((code = cmap_lookup(0, *((*start)++), tounicode_cmap)) != CID_NOTDEF) {
          ustring[ulen++] = code >> 8;
          ustring[ulen++] = code & 0xFF;
        } else if (*start < end && (code = cmap_lookup(*(*start-1), **start, tounicode_cmap)) != CID_NOTDEF) {
          ustring[ulen++] = code >> 8;
          ustring[ulen++] = code & 0xFF;
          (*start)++;
        }
      }
    if (*start < end) {
      (*start)++;
      result = pdf_new_stringx(ustring, ulen);
      RELEASE (ustring);
      return result;
    }
  }
  fprintf(stderr, "\nCould not find a literal string object to be converted to Unicode.\n");
  dump(*start = save, end);
  return NULL;
}

static pdf_obj *parse_pdf_hex_string_tounicode (char **start, char *end)
{
  register pdf_obj *result;
  register unsigned char *string, *ustring;
  register unsigned short code;
  register int ulen = 0, i = 0;
  int len;

  skip_white(start, end); save = *start;

  if (*start < end && **start == '<' &&
      (string = parse_hexadecimal(start, end, &len))) {
    /* Do nothing if the string is already Unicode. */
    if (len > 1 && string[0] == 0xFE && string[1] == 0xFF)
      result = pdf_new_stringx(string, len);
    else {
      ustring = NEW (len * 2 + 2, unsigned char);
      ustring[ulen++] = 0xFE; ustring[ulen++] = 0xFF;
      while (i < len)
        if ((code = cmap_lookup(0, string[i++], tounicode_cmap)) != CID_NOTDEF) {
          ustring[ulen++] = code >> 8;
          ustring[ulen++] = code & 0xFF;
        } else if (i < len && (code = cmap_lookup(string[i-1], string[i], tounicode_cmap)) != CID_NOTDEF) {
          ustring[ulen++] = code >> 8;
          ustring[ulen++] = code & 0xFF;
          i++;
        }
      result = pdf_new_stringx(ustring, ulen);
    }
    RELEASE (string);
    return result;
  }
  fprintf(stderr, "\nCould not find a hexadecimal string object to be converted to Unicode.\n");
  dump(*start = save, end);
  return NULL;
}

pdf_obj *parse_pdf_text_string (char **start, char *end)
{
  /* Turn off the warning message. */
  cmap_set_lookup(CMAP_NO_WARN_INVALID | CMAP_NO_WARN_MISSING);

  skip_white(start, end); save = *start;

  if (*start < end)
    switch (**start) {
    case '<': 
      return (tounicode_cmap < 0 ? parse_pdf_hex_string(start, end) :
                         parse_pdf_hex_string_tounicode(start, end));
    case '(':
      return (tounicode_cmap < 0 ? parse_pdf_string(start, end) :
                         parse_pdf_string_tounicode(start, end));
    }
  fprintf(stderr, "\nCould not find a text string object.");
  dump(*start = save, end);
  return NULL;
}
#endif

pdf_obj *parse_pdf_ann_dict (char **start, char *end)
{
  register pdf_obj *result, *tmp1, *tmp2;
  register char *name;

  skip_white(start, end); save = *start;

  if (*((*start)++) == '<' && *((*start)++) == '<') {
    result = pdf_new_dict();
    skip_white(start, end);
    while (*start < end && **start != '>') {
      if ((tmp1 = parse_pdf_name(start, end)) == NULL) {
        pdf_release_obj(result); 
        fprintf(stderr, "[parse] pdf_ann_dict: Could not find a name object.");
        dump(*start = save, end);
        return NULL;
      }
      /*
       * Check the type of the parsed name is the text string object.
       * See PDF Reference 1.4, p.490-492, Table 8.10.
       */
      name = pdf_string_value(tmp1);
#ifndef WITHOUT_TOUNICODE
      if (strcmp(name, "Contents") == 0)
      /* TODO: some problem occurred with ConTeXt in the case of "T" */
        tmp2 = parse_pdf_text_string(start, end);
      /* TODO
      else if (!strcmp(name, "A"))
        tmp2 = parse_pdf_action_dict(start, end);
      */
      else
#endif
        tmp2 = parse_pdf_object(start, end);
      if (tmp2 == NULL) {
        pdf_release_obj(result);
        pdf_release_obj(tmp1); 
        fprintf(stderr, "[parse] pdf_ann_dict: Could not find a valid object for /%s.", name);
        dump(*start = save, end);
        return NULL;
      }
      pdf_add_dict(result, tmp1, tmp2);
      skip_white(start, end);
    }
    if (*((*start)++) == '>' && *((*start)++) == '>')
      return result;
    else {
      pdf_release_obj(result);
      fprintf(stderr, "[parse] pdf_ann_dict: Dictionary object ended prematurely.\n");
      dump(*start = save, end);
      return NULL;
    }
  }
  fprintf (stderr, "[parse] pdf_ann_dict: Could not find a dictionary object.");
  dump(*start = save, end);
  return NULL;
}

pdf_obj *parse_pdf_out_dict (char **start, char *end)
{
  register pdf_obj *result, *tmp1, *tmp2;
  register char *name;

  skip_white(start, end); save = *start;

  if (*((*start)++) == '<' && *((*start)++) == '<') {
    result = pdf_new_dict();
    skip_white(start, end);
    while (*start < end && **start != '>') {
      if ((tmp1 = parse_pdf_name(start, end)) == NULL) {
        pdf_release_obj(result); 
        fprintf(stderr, "[parse] pdf_out_dict: Could not find a name object.");
        dump(*start = save, end);
        return NULL;
      }
      /*
       * Check the type of the parsed name is the text string object.
       * See PDF Reference 1.4, p.478, Table 8.4.
       */
      name = pdf_string_value(tmp1);
#ifndef WITHOUT_TOUNICODE
      if (strcmp(name, "Title") == 0)
        tmp2 = parse_pdf_text_string(start, end);
      /* TODO
      else if (!strcmp(name, "A"))
        tmp2 = parse_pdf_action_dict(start, end);
      */
      else
#endif
        tmp2 = parse_pdf_object(start, end);
      if (tmp2 == NULL) {
        pdf_release_obj(result);
        pdf_release_obj(tmp1); 
        fprintf(stderr, "[parse] pdf_out_dict: Could not find a valid object for /%s.", name);
        dump(*start = save, end);
        return NULL;
      }
      pdf_add_dict(result, tmp1, tmp2);
      skip_white(start, end);
    }
    if (*((*start)++) == '>' && *((*start)++) == '>')
      return result;
    else {
      pdf_release_obj(result);
      fprintf(stderr, "[parse] pdf_out_dict: Dictionary object ended prematurely.\n");
      dump(*start = save, end);
      return NULL;
    }
  }
  fprintf (stderr, "[parse] pdf_out_dict: Could not find a dictionary object.");
  dump(*start = save, end);
  return NULL;
}

pdf_obj *parse_pdf_info_dict (char **start, char *end)
{
  register pdf_obj *result, *tmp1, *tmp2;
  register char *name;

  skip_white(start, end); save = *start;

  if (*((*start)++) == '<' && *((*start)++) == '<') {
    result = pdf_new_dict();
    skip_white(start, end);
    while (*start < end && **start != '>') {
      if ((tmp1 = parse_pdf_name(start, end)) == NULL) {
        pdf_release_obj(result); 
        fprintf(stderr, "[parse] pdf_info_dict: Could not find a name object.");
        dump(*start = save, end);
        return NULL;
      }
      /*
       * Check the type of the parsed name is the text string object.
       * See PDF Reference 1.4, p.576, Table 9.2.
       */
      name = pdf_string_value(tmp1);
#ifndef WITHOUT_TOUNICODE
      if (strcmp(name, "Title") == 0 || strcmp(name, "Author") == 0 ||
          strcmp(name, "Subject") == 0 || strcmp(name, "Keywords") == 0 ||
          strcmp(name, "Creator") == 0 || strcmp(name, "Producer") == 0)
        tmp2 = parse_pdf_text_string(start, end);
      /* TODO
      else if (!strcmp(name, "A"))
        tmp2 = parse_pdf_action_dict(start, end);
      */
      else
#endif
        tmp2 = parse_pdf_object(start, end);
      if (tmp2 == NULL) {
        pdf_release_obj(result);
        pdf_release_obj(tmp1); 
        fprintf(stderr, "[parse] pdf_info_dict: Could not find a valid object for /%s.", name);
        dump(*start = save, end);
        return NULL;
      }
      pdf_add_dict(result, tmp1, tmp2);
      skip_white(start, end);
    }
    if (*((*start)++) == '>' && *((*start)++) == '>')
      return result;
    else {
      pdf_release_obj(result);
      fprintf(stderr, "[parse] pdf_info_dict: Dictionary object ended prematurely.\n");
      dump(*start = save, end);
      return NULL;
    }
  }
  fprintf (stderr, "[parse] pdf_info_dict: Could not find a dictionary object.");
  dump(*start = save, end);
  return NULL;
}

pdf_obj *parse_pdf_dict (char **start, char *end)
{
  register pdf_obj *result, *tmp1, *tmp2;

  skip_white(start, end); save = *start;

  if (*((*start)++) == '<' && *((*start)++) == '<') {
    result = pdf_new_dict();
    skip_white(start, end);
    while (*start < end && **start != '>') {
      if ((tmp1 = parse_pdf_name(start, end)) == NULL) {
        pdf_release_obj(result); 
        fprintf(stderr, "\nCould not find a name object in this dictionary object.");
        dump(*start = save, end);
        return NULL;
      }
      if ((tmp2 = parse_pdf_object(start, end)) == NULL) {
        pdf_release_obj(result);
        pdf_release_obj(tmp1); 
        fprintf(stderr, "\nCould not find a valid object in this dictionary object.");
        dump(*start = save, end);
        return NULL;
      }
      pdf_add_dict(result, tmp1, tmp2);
      skip_white(start, end);
    }
    if (*((*start)++) == '>' && *((*start)++) == '>')
      return result;
    else {
      pdf_release_obj(result);
      fprintf(stderr, "\nDictionary object ended prematurely.\n");
      dump(*start = save, end);
      return NULL;
    }
  }
  fprintf (stderr, "\nCould not find a dictionary object.");
  dump(*start = save, end);
  return NULL;
}

pdf_obj *parse_pdf_array (char **start, char *end)
{
  register pdf_obj *result, *tmp1;

  skip_white(start, end); save = *start;

  if (*((*start)++) == '[') {
    result = pdf_new_array();
    skip_white(start, end);
    while (*start < end && **start != ']') {
      if ((tmp1 = parse_pdf_object(start, end)) == NULL) {
        pdf_release_obj(result); 
        fprintf(stderr, "\nCould not find a valid object in this array object.");
        dump(*start = save, end);
        return NULL;
      }
      pdf_add_array(result, tmp1);
      skip_white(start, end);
    }
    if (*start < end) {
      (*start)++;
      return result;
    } else {
      pdf_release_obj(result);
      fprintf(stderr, "\nArray object ended prematurely.\n");
      dump(*start = save, end);
      return NULL;
    }
  }
  fprintf (stderr, "\nCould not find an array object.");
  dump(*start = save, end);
  return NULL;
}

static pdf_obj *parse_pdf_stream (char **start, char *end, pdf_obj *dict)
{
  register pdf_obj *result, *new_dict, *tmp1, *length_obj;
  unsigned long len;

/*
  if (pdf_lookup_dict(dict, "F")) {
    fprintf(stderr, "File streams not implemented (yet)");
    return NULL;
  }
*/

  if ((tmp1 = pdf_lookup_dict(dict, "Length")) == NULL) {
    fprintf(stderr, "No length specified");
    return NULL;
  }

  len = (unsigned long)pdf_number_value(length_obj = pdf_deref_obj(tmp1));
  pdf_release_obj(length_obj);

  skip_white(start, end); save = *start;
  if (end - *start >= 6 && !strncmp(*start, "stream", 6)) {
    *start += 6;
    if (*start < end && **start == '\r') (*start)++;
    if (*start < end && **start == '\n') (*start)++;
/*  skip_line(start, end);  */
    result = pdf_new_stream(0);
    new_dict = pdf_stream_dict(result);
    pdf_merge_dict(new_dict, dict);
    pdf_release_obj(dict);
    pdf_add_stream(result, *start, len);

    *start += len;
    skip_white(start, end);

    if (end - *start >= 9 && !strncmp(*start, "endstream", 9)) {
      *start += 9;
      return result;
    } else {
      fprintf(stderr, "\nStream object ended prematurely.\n");
      dump(*start = save, end);
      return NULL;
    }
  }
  fprintf(stderr, "\nCould not find a stream object.\n");
  dump(*start = save, end);
  return NULL;
}

#ifndef WITHOUT_TOUNICODE
static pdf_obj *parse_pdf_reference (char **start, char *end)
{
  register pdf_obj *result;
  register char *name;

  skip_white(start, end); save = *start;

  if ((name = parse_opt_ident(start, end))) {
    if ((result = lookup_reference(name)) == NULL) {
      fprintf(stderr, "\nCould not find the named reference (@%s).\n", name);
      dump(*start = save, end);
    }
    RELEASE (name);
    return result;
  }
  fprintf(stderr, "\nCould not find a reference name.\n");
  dump(*start = save, end);
  return NULL;
}
#endif

pdf_obj *parse_pdf_object (char **start, char *end)
{
  register pdf_obj *result, *num_obj1, *num_obj2;
  register char *tmp_pos;

  skip_white(start, end); save = *start;

  if (*start < end)
    switch (**start) {
    case '<': 
      if (*(*start+1) != '<')
        /* Hexadecimal string object */
        return parse_pdf_hex_string(start, end);
      result = parse_pdf_dict(start, end);
      skip_white(start, end);
      if (end - *start >= 6 && !strncmp(*start, "stream", 6))
        /* Stream object */
        return parse_pdf_stream(start, end, result);
      else
        /* Dictionary object */
        return result;
    case '(':
      /* Literal string object */
      return parse_pdf_string(start, end);
    case '[':
      /* Array object */
      return parse_pdf_array(start, end);
    case '/':
      /* Name object */
      return parse_pdf_name(start, end);
#ifndef WITHOUT_TOUNICODE
    case '@':
      /* Reference for internal use only */
      return parse_pdf_reference(start, end);
#endif
    case 'n':
      /* Null object */
      return parse_pdf_null(start, end);
    case 't':
    case 'f':
      /* Boolean object */
      return parse_pdf_boolean(start, end);
    case '+':
    case '-':
    case '.':
      /* Numeric object */
      return parse_pdf_number(start, end);
    default:
      if (isdigit(**start)) {
        num_obj1 = parse_pdf_number(start, end);
        skip_white(start, end);
        tmp_pos = *start;
        if (*start < end && isdigit(**start))
	  num_obj2 = parse_pdf_number(start, end);
	else
          /* Numeric object */
          return num_obj1;
        skip_white(start, end);
        if (*start < end && *((*start)++) == 'R') {
	  result = pdf_new_ref((unsigned long)pdf_number_value(num_obj1), 
			       (int)pdf_number_value(num_obj2));
	  pdf_release_obj(num_obj1);
	  pdf_release_obj(num_obj2);
          /* Indirect object */
	  return result;
        }
	pdf_release_obj(num_obj2);
	*start = tmp_pos;
        /* Numeric object */
        return num_obj1;
      }
    }
  fprintf (stderr, "\nCould not find any valid object.");
  dump(*start = save, end);
  return NULL;
}

char *parse_c_string (char **start, char *end)
{
  register unsigned char *result;
  register int len = 0;

  skip_white(start, end); save = *start;

  if (*start < end && **start == '"') {
    (*start)++;
    result = NEW (end - *start + 1, unsigned char);
    while (*start < end && **start != '"')
      if (**start == '\\' &&
          parse_escape_char(start, end, "\\\"", &(result[len]))) len++;
      else
        result[len++] = *((*start)++);
    if (*start < end) {
      (*start)++;
      result[len] = 0;
      return (char *)result;
    }
  }
  fprintf(stderr, "\nCould not find a C string.\n");
  dump(*start = save, end);
  return NULL;
}

void parse_key_val (char **start, char *end, char **key, char **val) 
{
  skip_white(start, end);
  if ((*key = parse_c_ident(start, end))) {
    skip_white(start, end);
    if (*start < end && **start == '=') {
      (*start)++;
      skip_white(start, end);
      if (*start < end) {
        if (**start == '"')
	  *val = parse_c_string(start, end);
        else
	  *val = parse_val_ident(start, end);
        return;
      }
    }
  }
  *val = NULL;
}
