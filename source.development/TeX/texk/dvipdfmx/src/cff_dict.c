/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/cff_dict.c,v 1.1 2004/01/08 13:44:13 rahtz Exp $
    
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
  CFF DICT data:
    See, Adobe Technical Note #5176 "The Compact Font Format Specification"
*/

/* rint */
#include <math.h>
/* strtod */
#include <stdlib.h>
/* errno, overflow/underflow in strtod() */
#include <errno.h>

#include "error.h"
#include "mem.h"

/* work_buffer for get_real() */
#include "mfileio.h"

#include "cff_types.h"
#include "cff_limits.h"

/* #include "cff_string.h" */
#include "cff_dict.h"

#define CFF_PARSE_OK                0
#define CFF_ERROR_PARSE_ERROR      -1
#define CFF_ERROR_STACK_OVERFLOW   -2
#define CFF_ERROR_STACK_UNDERFLOW  -3
#define CFF_ERROR_STACK_RANGECHECK -4

/*
  CFF Dictionary
   I won't use linked list here.
*/
#define DICT_ENTRY_MAX 16
cff_dict *cff_new_dict (void)
{
  cff_dict *dict;

  dict = NEW(1, cff_dict);
  dict->max     = DICT_ENTRY_MAX;
  dict->count   = 0;
  dict->entries = NEW(dict->max, cff_dict_entry);

  return dict;
}

void cff_release_dict (cff_dict *dict)
{
  if (dict) {
    if (dict->entries) {
      int i;
      for (i=0;i<dict->count;i++) {
	if ((dict->entries)[i].values)
	  RELEASE((dict->entries)[i].values);
      }
      RELEASE(dict->entries);
    }
  RELEASE(dict);
  }
}

/*
  Operand stack:
   only numbers are stored (as double)

  Operand types:

   number : double (integer or real)
   boolean: N/A (stored as number)
   SID    : N/A (stored as number)
   array  : N/A (numbers)
   delta  : N/A (numbers)
*/

#define CFF_DICT_STACK_LIMIT 64
static int    stack_top = 0;
static double arg_stack[CFF_DICT_STACK_LIMIT];

/*
  CFF DICT encoding:
   TODO: default values
*/

#define CFF_LAST_DICT_OP1 22
#define CFF_LAST_DICT_OP2 39
#define CFF_LAST_DICT_OP (CFF_LAST_DICT_OP1 + CFF_LAST_DICT_OP2)

static struct {
  char *opname;
  int   argtype;
} dict_operator[CFF_LAST_DICT_OP] = {
  {"version",     CFF_TYPE_SID},
  {"Notice",      CFF_TYPE_SID},
  {"FullName",    CFF_TYPE_SID},
  {"FamilyName",  CFF_TYPE_SID},
  {"Weight",      CFF_TYPE_SID},
  {"FontBBox",    CFF_TYPE_ARRAY},
  {"BlueValues",       CFF_TYPE_DELTA},
  {"OtherBlues",       CFF_TYPE_DELTA},
  {"FamilyBlues",      CFF_TYPE_DELTA},
  {"FamilyOtherBlues", CFF_TYPE_DELTA},
  {"StdHW",            CFF_TYPE_NUMBER},
  {"StdVW",            CFF_TYPE_NUMBER},
  {NULL, -1},  /* first byte of two-byte operator */
  /* Top */
  {"UniqueID",    CFF_TYPE_NUMBER},
  {"XUID",        CFF_TYPE_ARRAY},
  {"charset",     CFF_TYPE_OFFSET},
  {"Encoding",    CFF_TYPE_OFFSET},
  {"CharStrings", CFF_TYPE_OFFSET},
  {"Private",     CFF_TYPE_SZOFF}, /* two numbers (size and offset) */
  /* Private */
  {"Subrs",         CFF_TYPE_OFFSET},
  {"defaultWidthX", CFF_TYPE_NUMBER},
  {"nominalWidthX", CFF_TYPE_NUMBER},
  /* Operator 2 */
  {"Copyright",          CFF_TYPE_SID},
  {"IsFixedPitch",       CFF_TYPE_BOOLEAN},
  {"ItalicAngle",        CFF_TYPE_NUMBER},
  {"UnderlinePosition",  CFF_TYPE_NUMBER},
  {"UnderlineThickness", CFF_TYPE_NUMBER},
  {"PaintType",      CFF_TYPE_NUMBER},
  {"CharstringType", CFF_TYPE_NUMBER},
  {"FontMatrix",     CFF_TYPE_ARRAY},
  {"StrokeWidth",    CFF_TYPE_NUMBER},
  {"BlueScale", CFF_TYPE_NUMBER},
  {"BlueShift", CFF_TYPE_NUMBER},
  {"BlueFuzz",  CFF_TYPE_NUMBER},
  {"StemSnapH", CFF_TYPE_DELTA},
  {"StemSnapV", CFF_TYPE_DELTA},
  {"ForceBold", CFF_TYPE_BOOLEAN},
  {NULL, -1},
  {NULL, -1},
  {"LanguageGroup",     CFF_TYPE_NUMBER},
  {"ExpansionFactor",   CFF_TYPE_NUMBER},
  {"InitialRandomSeed", CFF_TYPE_NUMBER},
  {"SyntheticBase", CFF_TYPE_NUMBER},
  {"PostScript",    CFF_TYPE_SID},
  {"BaseFontName",  CFF_TYPE_SID},
  {"BaseFontBlend", CFF_TYPE_DELTA}, /* MMaster ? */
  {NULL, -1},
  {NULL, -1},
  {NULL, -1},
  {NULL, -1},
  {NULL, -1},
  {NULL, -1},
  /* CID-Keyed font */
  {"ROS",             CFF_TYPE_ROS}, /* SID SID number */
  {"CIDFontVersion",  CFF_TYPE_NUMBER},
  {"CIDFontRevision", CFF_TYPE_NUMBER},
  {"CIDFontType",     CFF_TYPE_NUMBER},
  {"CIDCount",        CFF_TYPE_NUMBER},
  {"UIDBase",         CFF_TYPE_NUMBER},
  {"FDArray",         CFF_TYPE_OFFSET},
  {"FDSelect",        CFF_TYPE_OFFSET},
  {"FontName",        CFF_TYPE_SID},
};

/* Parse DICT data */
static double get_integer (card8 **data, card8 *endptr, int *status)
{
  long result = 0;
  card8 b0, b1, b2;

  b0 = *(*data)++;
  if (b0 == 28 && *data < endptr - 2) { /* shortint */
    b1 = *(*data)++;
    b2 = *(*data)++;
    result = b1*256+b2;
    if (result > 0x7fffL)
      result -= 0x10000L;
  } else if (b0 == 29 && *data < endptr - 4) { /* longint */
    int i;
    result = *(*data)++;
    if (result > 0x7f)
      result -= 0x100;
    for (i=0;i<3;i++) {
      result = result*256+(**data);
      *data += 1;
    }
  } else if (b0 >= 32 && b0 <= 246) { /* int (1) */
    result = b0 - 139;
  } else if (b0 >= 247 && b0 <= 250) { /* int (2) */
    b1 = *(*data)++;
    result = (b0-247)*256+b1+108;
  } else if (b0 >= 251 && b0 <= 254) {
    b1 = *(*data)++;
    result = -(b0-251)*256-b1-108;
  } else {
    *status = CFF_ERROR_PARSE_ERROR;
  }

  return (double) result;
}

/* Simply uses strtod */
static double get_real(card8 **data, card8 *endptr, int *status)
{
  double result = 0.0;
  int nibble = 0, pos = 0;
  int len = 0, fail = 0;

  if (**data != 30 || *data >= endptr -1) {
    *status = CFF_ERROR_PARSE_ERROR;
    return 0.0;
  }

  *data += 1; /* skip first byte (30) */

  pos = 0;
  while ((! fail) && len < WORK_BUFFER_SIZE - 2 && *data < endptr) {
    /* get nibble */
    if (pos % 2) {
      nibble = **data & 0x0f;
      *data += 1;
    } else {
      nibble = (**data >> 4) & 0x0f;
    }
    if (nibble >= 0x00 && nibble <= 0x09) {
      work_buffer[len++] = nibble + '0';
    } else if (nibble == 0x0a) { /* . */
      work_buffer[len++] = '.';
    } else if (nibble == 0x0b || nibble == 0x0c) { /* E, E- */
      work_buffer[len++] = 'e';
      if (nibble == 0x0c)
	work_buffer[len++] = '-';
    } else if (nibble == 0x0e) { /* `-' */
      work_buffer[len++] = '-';
    } else if (nibble == 0x0d) { /* skip */
      /* do nothing */
    } else if (nibble == 0x0f) { /* end */
      work_buffer[len++] = '\0';
      if (((pos % 2) == 0) && (**data != 0xff)) {
	fail = 1;
      }
      break;
    } else { /* invalid */
      fail = 1;
    }
    pos++;
  }

  /* returned values */
  if (fail || nibble != 0x0f) {
    *status = CFF_ERROR_PARSE_ERROR;
  } else {
    char *s;
    result = strtod(work_buffer, &s);
    if (*s != 0 || errno == ERANGE) {
      *status = CFF_ERROR_PARSE_ERROR;
    }
  }

  return result;
}

/* operators */
static void add_dict (cff_dict *dict,
		      card8 **data, card8 *endptr, int *status)
{
  int id, argtype;

  id = **data;
  if (id == 0x0c) {
    *data += 1;
    if (*data >= endptr ||
	(id = **data + CFF_LAST_DICT_OP1) >= CFF_LAST_DICT_OP) {
      *status = CFF_ERROR_PARSE_ERROR;
      return;
    }
  } else if (id >= CFF_LAST_DICT_OP1) {
    *status = CFF_ERROR_PARSE_ERROR;
    return;
  }

  argtype = dict_operator[id].argtype;
  if (dict_operator[id].opname == NULL || argtype < 0) {
    *status = CFF_ERROR_PARSE_ERROR;
    return;
  } else if (stack_top < 1) {
    *status = CFF_ERROR_STACK_UNDERFLOW;
    return;
  }

  if (dict->count >= dict->max) {
    dict->max += DICT_ENTRY_MAX;
    dict->entries = RENEW(dict->entries, dict->max, cff_dict_entry);
  }

  (dict->entries)[dict->count].id = id;
  (dict->entries)[dict->count].key = dict_operator[id].opname;
  if (argtype == CFF_TYPE_NUMBER ||
      argtype == CFF_TYPE_BOOLEAN ||
      argtype == CFF_TYPE_SID ||
      argtype == CFF_TYPE_OFFSET) {
    stack_top--;
    (dict->entries)[dict->count].count  = 1;
    (dict->entries)[dict->count].values = NEW(1, double);
    (dict->entries)[dict->count].values[0] = arg_stack[stack_top];
  } else {
    (dict->entries)[dict->count].count  = stack_top;
    (dict->entries)[dict->count].values = NEW(stack_top, double);
    while (stack_top > 0) {
      stack_top--;
      (dict->entries)[dict->count].values[stack_top] = arg_stack[stack_top];
    }
  }

  dict->count += 1;
  *data += 1;

  return;
}

/*
  All operands are treated as number or array of numbers.
   Private takes two numbers, size and offset
   ROS takes three numbers, SID, SID, and a number
*/
cff_dict *cff_dict_unpack (card8 *data, card8 *endptr)
{
  cff_dict *dict;
  int status = CFF_PARSE_OK;

  stack_top = 0;

  dict = cff_new_dict();
  while (data < endptr && status == CFF_PARSE_OK) {
    if (*data < 22) { /* operator */
      add_dict(dict, &data, endptr, &status);
    } else if (*data == 30) { /* real - First byte of a sequence (variable) */
      if (stack_top < CFF_DICT_STACK_LIMIT) {
	arg_stack[stack_top] = get_real(&data, endptr, &status);
	stack_top++;
      } else {
	status = CFF_ERROR_STACK_OVERFLOW;
      }
    } else if (*data == 255 || (*data >= 22 && *data <= 27)) { /* reserved */
      data++;
    } else { /* everything else are integer */
      if (stack_top < CFF_DICT_STACK_LIMIT) {
	arg_stack[stack_top] = get_integer(&data, endptr, &status);
	stack_top++;
      } else {
	status = CFF_ERROR_STACK_OVERFLOW;
      }
    }
  }

  if (status != CFF_PARSE_OK) {
    fprintf(stderr, "\nin cff_dict_unpack(): error %d\n", status);
    ERROR("Parsing CFF DICT failed.");
  } else if (stack_top != 0) {
    fprintf(stderr, "\n** Warning: Garbage in CFF DICT data **\n");
    stack_top = 0;
  }

  return dict;
}

/* Pack DICT data */
static long pack_integer (card8 *dest, long destlen, long value)
{
  long len = 0;

  if (value >= -107 && value <= 107) {
    if (destlen < 1)
      ERROR("in pack_integer(): Buffer overflow");
    dest[0] = (value + 139) & 0xff;
    len = 1;
  } else if (value >= 108 && value <= 1131) {
    if (destlen < 2)
      ERROR("in pack_integer(): Buffer overflow");
    value = 0xf700u + value - 108;
    dest[0] = (value >> 8) & 0xff;
    dest[1] = value & 0xff;
    len = 2;
  } else if (value >= -1131 && value <= -108) {
    if (destlen < 2)
      ERROR("in pack_integer(): Buffer overflow");
    value = 0xfb00u - value - 108;
    dest[0] = (value >> 8) & 0xff;
    dest[1] = value & 0xff;
    len = 2;
  } else if (value >= -32768 && value <= 32767) { /* shortint */
    if (destlen < 3)
      ERROR("in pack_integer(): Buffer overflow");
    dest[0] = 28;
    dest[1] = (value >> 8) & 0xff;
    dest[2] = value & 0xff;
    len = 3;
  } else { /* longint */
    if (destlen < 5)
      ERROR("in pack_integer(): Buffer overflow");
    dest[0] = 29;
    dest[1] = (value >> 24) & 0xff;
    dest[2] = (value >> 16) & 0xff;
    dest[3] = (value >> 8) & 0xff;
    dest[4] = value & 0xff;
    len = 5;
  }

  return len;
}

static long pack_real (card8 *dest, long destlen, double value)
{
  long exp;
  int i = 0, pos = 2;
#define CFF_REAL_MAX_LEN 17

  if (destlen < 2)
    ERROR("in pack_real(): Buffer overflow");

  dest[0] = 30;

  if (value == 0.0) {
    dest[1] = 0x0f;
    return 2;
  }

  if (value < 0.0) {
    dest[1] = 0xe0;
    value *= -1.0;
    pos++;
  }

  exp = 0;
  if (value >= 10.0) {
    while (value >= 10.0) {
      value /= 10.0;
      exp++;
    }
  } else if (value < 1.0) {
    while (value < 1.0) {
      value *= 10.0;
      exp--;
    }
  }

  sprintf(work_buffer, "%1.14g", value);
  for (i=0;i<CFF_REAL_MAX_LEN;i++) {
    unsigned char ch = 0;
    if (work_buffer[i] == '\0') {
      break;
    } else if (work_buffer[i] == '.') {
      ch = 0x0a;
    } else if (work_buffer[i] >= '0' && work_buffer[i] <= '9') {
      ch = work_buffer[i] - '0';
    } else {
      ERROR("in pack_real(): Invalid character");
    }

    if (destlen < pos/2 + 1)
      ERROR("in pack_real(): Buffer overflow");

    if (pos % 2) {
      dest[pos/2] += ch;
    } else {
      dest[pos/2] = (ch << 4);
    }
    pos++;
  }

  if (exp > 0) {
    if (pos % 2) {
      dest[pos/2] += 0x0b;
    } else {
      if (destlen < pos/2 + 1)
	ERROR("in pack_real(): Buffer overflow");
      dest[pos/2] = 0xb0;
    }
    pos++;
  } else if (exp < 0) {
    if (pos % 2) {
      dest[pos/2] += 0x0c;
    } else {
      if (destlen < pos/2 + 1)
	ERROR("in pack_real(): Buffer overflow");
      dest[pos/2] = 0xc0;
    }
    exp *= -1;
    pos++;
  }

  if (exp != 0) {
    sprintf(work_buffer, "%ld", exp);
    for (i=0;i<CFF_REAL_MAX_LEN;i++) {
      unsigned char ch = 0;
      if (work_buffer[i] == '\0') {
	break;
      } else if (work_buffer[i] == '.') {
	ch = 0x0a;
      } else if (work_buffer[i] >= '0' && work_buffer[i] <= '9') {
	ch = work_buffer[i] - '0';
      } else {
	ERROR("in pack_real(): Invalid character");
      }

      if (destlen < pos/2 + 1)
	ERROR("in pack_real(): Buffer overflow");

      if (pos % 2) {
	dest[pos/2] += ch;
      } else {
	dest[pos/2] = (ch << 4);
      }
      pos++;
    }
  }

  if (pos % 2) {
    dest[pos/2] += 0x0f;
    pos++;
  } else {
    if (destlen < pos/2 + 1)
      ERROR("in pack_real(): Buffer overflow");
    dest[pos/2] = 0xff;
    pos += 2;
  }

  return pos/2;
}

static long cff_dict_put_number (double value, card8 *dest, long destlen,
				 int type)
{
  long len;
  double ipart;

#ifdef WIN32
  ipart = floor(value);
#else
  ipart = rint(value);
#endif
  /* set offset to longint */
  if (type == CFF_TYPE_OFFSET) {
    long lvalue = (long) value;
    if (destlen < 5)
      ERROR("in pack_integer(): Buffer overflow");
    dest[0] = 29;
    dest[1] = (lvalue >> 24) & 0xff;
    dest[2] = (lvalue >> 16) & 0xff;
    dest[3] = (lvalue >> 8) & 0xff;
    dest[4] = lvalue & 0xff;
    len = 5;
  } else if (value > CFF_INT_MAX || value < CFF_INT_MIN ||
	     (fabs(value - ipart) > 1.0e-15)) { /* real */
    len = pack_real(dest, destlen, value);
  } else { /* integer */
    len = pack_integer(dest, destlen, (long) value);
  }

  return len;
}

long cff_dict_pack (cff_dict *dict, card8 *dest, long destlen)
{
  long len = 0;
  int type, id;
  int i, ii;

  for (i=0;i<dict->count;i++) {
    if ((dict->entries)[i].count > 0) {
      id = (dict->entries)[i].id;
      if (dict_operator[id].argtype == CFF_TYPE_OFFSET ||
	  dict_operator[id].argtype == CFF_TYPE_SZOFF) {
	type = CFF_TYPE_OFFSET;
      } else {
	type = CFF_TYPE_NUMBER;
      }
      for (ii=0;ii<(dict->entries)[i].count;ii++) {
	len += cff_dict_put_number((dict->entries)[i].values[ii],
				   dest+len,
				   destlen-len, type);
      }
      if (id >= 0 && id < CFF_LAST_DICT_OP1) {
	if (len + 1 > destlen)
	  ERROR("in cff_dict_pack(): Buffer overflow");
	dest[len++] = id;
      } else if (id >= 0 && id < CFF_LAST_DICT_OP) {
	if (len + 2 > destlen)
	  ERROR("in cff_dict_pack(): Buffer overflow");
	dest[len++] = 12;
	dest[len++] = id - CFF_LAST_DICT_OP1;
      } else {
	ERROR("in cff_dict_pack(): Invalid operator ID");
      }
    }
  }

  return len;
}

void cff_dict_add (cff_dict *dict, char *key, int count)
{
  int id, i;

  for (id=0;id<CFF_LAST_DICT_OP;id++) {
    if (key && dict_operator[id].opname &&
	strcmp(dict_operator[id].opname, key) == 0)
      break;
  }

  if (id == CFF_LAST_DICT_OP)
    ERROR("Invalid operator");

  for (i=0;i<dict->count;i++) {
    if ((dict->entries)[i].id == id) {
      if ((dict->entries)[i].count != count)
	ERROR("Inconsistent DICT argument number");
      return;
    }
  }

  if (dict->count + 1 >= dict->max) {
    dict->max += 8;
    dict->entries = RENEW(dict->entries, dict->max, cff_dict_entry);
  }

  (dict->entries)[dict->count].id    = id;
  (dict->entries)[dict->count].key   = dict_operator[id].opname;
  (dict->entries)[dict->count].count = count;
  if (count > 0) {
    (dict->entries)[dict->count].values = NEW(count, double);
  } else {
    (dict->entries)[dict->count].values = NULL;
  }
  dict->count += 1;

  return;
}

void cff_dict_remove (cff_dict *dict, char *key)
{
  int i;
  for (i=0;i<dict->count;i++) {
    if (key && strcmp(key, (dict->entries)[i].key) == 0) {
      (dict->entries)[i].count = 0;
      if ((dict->entries)[i].values)
	RELEASE((dict->entries)[i].values);
      (dict->entries)[i].values = NULL;
    }
  }
}

int cff_dict_known (cff_dict *dict, char *key)
{
  int i;

  for (i=0;i<dict->count;i++) {
    if (key && strcmp(key, (dict->entries)[i].key) == 0
	&& (dict->entries)[i].count > 0)
      return 1;
  }

  return 0;
}

double cff_dict_get (cff_dict *dict, char *key, int idx)
{
  double value = 0.0;
  int i;

  if (key == NULL || dict == NULL)
    ERROR("in cff_dict_get(): Passed invalid data");

  for (i=0;i<dict->count;i++) {
    if (strcmp(key, (dict->entries)[i].key) == 0) {
      if ((dict->entries)[i].count > idx) {
	value = (dict->entries)[i].values[idx];
      } else {
	ERROR("in cff_dict_get(): Invalid index number");
      }
      break;
    }
  }

  if (i == dict->count) {
    fprintf(stderr, "\nDICT entry `%s' not found.\n", key);
    ERROR("Cannot continue");
  }

  return value;
}

void cff_dict_set (cff_dict *dict, char *key, int idx, double value)
{
  int i;

  if (key == NULL || dict == NULL)
    ERROR("in cff_dict_set(): Passed invalid data");

  for (i=0;i<dict->count;i++) {
    if (strcmp(key, (dict->entries)[i].key) == 0) {
      if ((dict->entries)[i].count > idx) {
	(dict->entries)[i].values[idx] = value;
      } else {
	ERROR("in cff_dict_set(): Invalid index number");
      }
      break;
    }
  }

  if (i == dict->count) {
    fprintf(stderr, "\nCFF DICT entry `%s' not found.\n", key);
    ERROR("Cannot continue");
  }
}
