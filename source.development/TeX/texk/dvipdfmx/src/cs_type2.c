/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/cs_type2.c,v 1.1 2004/01/08 13:44:13 rahtz Exp $
    
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
  Type 2 Charstring support:
   Decode and encode Type 2 charstrings

   All local/global subroutine calls in a given charstring are replace by
   content of subroutine charstrings. I do that because some PostScript RIP
   seems having problems with sparse subroutine array. Workaround for this is
   to re-order subroutine array so that no gap appears in the subroutine
   array, or put dummy charstrings that contains only `return' in the gap.
   However, remapping subroutine number is difficult for Type 2 charstrings
   due to the bias which depends on the total number of subroutines. I think
   expanding all subroutines in charstrings is more efficeint than putting
   subroutines that contains only `return' in the case of subsetted font.
   Adobe distiller seems doing same thing.

   I've not implemented the `random' operator which generates a pseudo-random
   number in the range (0, 1] and push them into argument stack.
   How pseudo-random sequences are generated is not documented in the Type 2
   charstring spec..
*/

#include <stdio.h>

/* fabs, rint, sqrt ... */
#include <math.h>

#include "error.h"

/* data types, limits */
#include "cff_types.h"
#include "cff_limits.h"

/* decoder/encoder status codes */
#define CS_BUFFER_ERROR -3
#define CS_STACK_ERROR  -2
#define CS_PARSE_ERROR  -1
#define CS_PARSE_OK      0
#define CS_PARSE_END     1
#define CS_SUBR_RETURN   2
#define CS_CHAR_END      3

static int status = CS_PARSE_ERROR;

#define DST_NEED(a,b) {if ((a) < (b)) { status = CS_BUFFER_ERROR ; return ; }}
#define SRC_NEED(a,b) {if ((a) < (b)) { status = CS_PARSE_ERROR  ; return ; }}
#define NEED(a,b)     {if ((a) < (b)) { status = CS_STACK_ERROR  ; return ; }}

/* hintmask and cntrmask need number of stem zones */
static int num_stems = 0;
static int phase     = 0;

/* subroutine nesting */
static int nest      = 0;

/* advance width */
static int    have_width = 0;
static double width      = 0.0;

/*
  Standard Encoding Accented Characters:
   Optional four arguments for endchar. See, CFF spec., p.35.
   Not supported yet.
*/
#if 0
/* adx ady bchar achar endchar */
static double seac[4] = {0.0, 0.0, 0.0, 0.0};
#endif

/* Operand stack and Transient array */
static int    stack_top = 0;
static double arg_stack[CS_ARG_STACK_MAX];
static double trn_array[CS_TRANS_ARRAY_MAX];

/*
  Type 2 CharString encoding
*/

/*
  1-byte CharString operaotrs
   cs_escape is first byte of two-byte operator
*/

/*      RESERVED      0 */
#define cs_hstem      1
/*      RESERVED      2 */
#define cs_vstem      3
#define cs_vmoveto    4
#define cs_rlineto    5
#define cs_hlineto    6
#define cs_vlineto    7
#define cs_rrcurveto  8
/*      cs_closepath  9  : TYPE1 */
#define cs_callsubr   10
#define cs_return     11
#define cs_escape     12
/*      cs_hsbw       13 : TYPE1 */
#define cs_endchar    14
/*      RESERVED      15 */
/*      RESERVED      16 */
/*      RESERVED      17 */
#define cs_hstemhm    18
#define cs_hintmask   19
#define cs_cntrmask   20
#define cs_rmoveto    21
#define cs_hmoveto    22
#define cs_vstemhm    23
#define cs_rcurveline 24
#define cs_rlinecurve 25
#define cs_vvcurveto  26
#define cs_hhcurveto  27
/*      SHORTINT      28 : first byte of shortint*/
#define cs_callgsubr  29
#define cs_vhcurveto  30
#define cs_hvcurveto  31

/*
  2-byte CharString operaotrs
   "dotsection" is obsoleted in Type 2 charstring.
*/
#define cs_dotsection 0
/*      cs_vstem3     1 : TYPE1 */
/*      cs_hstem3     2 : TYPE1 */
#define cs_and        3
#define cs_or         4
#define cs_not        5
/*      cs_seac       6 : TYPE1 */
/*      cs_sbw        7 : TYPE1 */
/*      RESERVED      8  */
#define cs_abs        9
#define cs_add        10
#define cs_sub        11
#define cs_div        12
/*      RESERVED      13 */
#define cs_neg        14
#define cs_eq         15
/*      cs_callothersubr 16 : TYPE1 */
/*      cs_pop           17 : TYPE1 */
#define cs_drop       18
/*      RESERVED      19 */
#define cs_put        20
#define cs_get        21
#define cs_ifelse     22 
#define cs_random     23
#define cs_mul        24
/*      RESERVED      25 */
#define cs_sqrt       26
#define cs_dup        27
#define cs_exch       28
#define cs_index      29
#define cs_roll       30
/*      cs_setcurrentpoint 31 : TYPE1 */
/*      RESERVED      32 */
/*      RESERVED      33 */
#define cs_hflex      34
#define cs_flex       35
#define cs_hflex1     36
#define cs_flex1      37

/*
  clear_stack() put all operands sotred in operand stack to dest.
*/
static void clear_stack (card8 **dest, card8 *limit)
{
  int i;

  for (i=0;i<stack_top;i++) {
    double value;
    long ivalue;
    value = arg_stack[i];
#ifdef WIN32
    ivalue = (long) floor(value);
#else
    ivalue = (long) rint(value);
#endif
    if (value >= 0x8000L || value <= (-0x8000L - 1)) {
      /*
	This number cannot be represented as a single operand.
	We must use `a b mul ...' or `a c div' to represent large values.
       */
      ERROR("in clear_stack(): argument value too large");
    } else if (fabs(value - ivalue) > 3.0e-5) {
      /* 16.16-bit signed fixed value  */
      DST_NEED(limit, *dest + 5);
      *(*dest)++ = 255;
      ivalue = (long) floor(value); /* mantissa */
      *(*dest)++ = (ivalue >> 8) & 0xff;
      *(*dest)++ = ivalue & 0xff;
      ivalue = (long)((value - ivalue) * 0x10000l); /* fraction */
      *(*dest)++ = (ivalue >> 8) & 0xff;
      *(*dest)++ = ivalue & 0xff;
      /* Everything else are integers. */
    } else if (ivalue >= -107 && ivalue <= 107) {
      DST_NEED(limit, *dest + 1);
      *(*dest)++ = ivalue + 139;
    } else if (ivalue >= 108 && ivalue <= 1131) {
      DST_NEED(limit, *dest + 2);
      ivalue = 0xf700u + ivalue - 108;
      *(*dest)++ = (ivalue >> 8) & 0xff;
      *(*dest)++ = ivalue & 0xff;
    } else if (ivalue >= -1131 && ivalue <= -108) {
      DST_NEED(limit, *dest + 2);
      ivalue = 0xfb00u - ivalue - 108;
      *(*dest)++ = (ivalue >> 8) & 0xff;
      *(*dest)++ = ivalue & 0xff;
    } else if (ivalue >= -32768 && ivalue <= 32767) { /* shortint */
      DST_NEED(limit, *dest + 3);
      *(*dest)++ = 28;
      *(*dest)++ = (ivalue >> 8) & 0xff;
      *(*dest)++ = (ivalue) & 0xff;
    } else { /* Shouldn't come here */
      ERROR("in clear_stack(): unexpected error");
    }
  }

  stack_top = 0; /* clear stack */

  return;
}

/*
  Single byte operators:
   Path construction, Operator for finishing a path, Hint operators.

   phase:
    0: inital state
    1: hint declaration, first stack-clearing operator appeared
    2: in path construction
*/

static void do_operator1 (card8 **dest, card8 *limit,
			  card8 **data, card8 *endptr)
{
  card8 op = **data;

  *data += 1;

  switch (op) {
  case cs_hstemhm:
  case cs_vstemhm:
  /* charstring may have hintmask if above operator have seen */
  case cs_hstem:
  case cs_vstem:
    if (phase == 0 && (stack_top % 2)) {
      have_width = 1;
      width = arg_stack[0];
    }
    num_stems += stack_top/2;
    clear_stack(dest, limit);
    DST_NEED(limit, *dest + 1);
    *(*dest)++ = op;
    phase = 1;
    break;
  case cs_hintmask:
  case cs_cntrmask:
    if (phase < 2) {
      if (phase == 0 && (stack_top % 2)) {
	have_width = 1;
	width = arg_stack[0];
      }
      num_stems += stack_top/2;
    }
    clear_stack(dest, limit);
    DST_NEED(limit, *dest + 1);
    *(*dest)++ = op;
    if (num_stems > 0) {
      int masklen = (num_stems + 7) / 8;
      DST_NEED(limit, *dest + masklen);
      SRC_NEED(endptr, *data + masklen);
      memmove(*dest, *data, masklen);
      *data += masklen;
      *dest += masklen;
    }
    phase = 2;
    break;
  case cs_rmoveto:
    if (phase == 0 && (stack_top % 2)) {
      have_width = 1;
      width = arg_stack[0];
    }
    clear_stack(dest, limit);
    DST_NEED(limit, *dest + 1);
    *(*dest)++ = op;
    phase = 2;
    break;
  case cs_hmoveto:
  case cs_vmoveto:
    if (phase == 0 && (stack_top % 2) == 0) {
      have_width = 1;
      width = arg_stack[0];
    }
    clear_stack(dest, limit);
    DST_NEED(limit, *dest + 1);
    *(*dest)++ = op;
    phase = 2;
    break;
  case cs_endchar:
    if (stack_top == 1) {
      have_width = 1;
      width = arg_stack[0];
      clear_stack(dest, limit);
    } else if (stack_top == 4 || stack_top == 5) {
      fprintf(stderr, "\n** Composite glyph not supported **\n");
      status = CS_PARSE_ERROR;
      return;
    } else if (stack_top > 0) {
      fprintf(stderr, "\n*** Operand stack not empty ***\n");
    }
    DST_NEED(limit, *dest + 1);
    *(*dest)++ = op;
    status = CS_CHAR_END;
    break;
  /* above oprators are candidate for first stack-clearing operator */
  case cs_rlineto:
  case cs_hlineto:
  case cs_vlineto:
  case cs_rrcurveto:
  case cs_rcurveline:
  case cs_rlinecurve:
  case cs_vvcurveto:
  case cs_hhcurveto:
  case cs_vhcurveto:
  case cs_hvcurveto:
    if (phase < 2) {
      fprintf(stderr, "\n*** Broken Type 2 charstring ***\n");
      status = CS_PARSE_ERROR;
      return;
    }
    clear_stack(dest, limit);
    DST_NEED(limit, *dest + 1);
    *(*dest)++ = op;
    break;
  /* all operotors above are stack-clearing operator */
  /* no output */
  case cs_return:
  case cs_callgsubr:
  case cs_callsubr:
    ERROR("in do_operator1(): unexpected call(g)subr/return");
    break;
  default:
    /* no-op ? */
    fprintf(stderr, "\n*** Unknown charstring operator: <%02x> ***\n", op);
    status = CS_PARSE_ERROR;
    break;
  }

  return;
}

/*
  Double byte operators:
   Flex, arithmetic, conditional, and storage operators.

   Following operators are not supported:
    random: How random ?
*/
static void do_operator2 (card8 **dest, card8 *limit,
			  card8 **data, card8 *endptr)
{
  card8 op;

  *data += 1;

  SRC_NEED(endptr, *data + 1);

  op = **data;
  *data += 1;

  switch(op) {
  case cs_dotsection: /* deprecated */
    fprintf(stderr, "`dotsection' deprecated in Type 2 charstring.");
    status = CS_PARSE_ERROR;
    return;
    break;
  case cs_hflex:
  case cs_flex:
  case cs_hflex1:
  case cs_flex1:
    if (phase < 2) {
      fprintf(stderr, "\n*** Broken Type 2 charstring ***\n");
      status = CS_PARSE_ERROR;
      return;
    }
    clear_stack(dest, limit);
    DST_NEED(limit, *dest + 2);
    *(*dest)++ = cs_escape;
    *(*dest)++ = op;
    break;
  /* all operator above are stack-clearing */
  /* no output */
  case cs_and:
    NEED(stack_top, 2);
    stack_top--;
    if (arg_stack[stack_top] && arg_stack[stack_top-1]) {
      arg_stack[stack_top-1] = 1.0;
    } else {
      arg_stack[stack_top-1] = 0.0;
    }
    break;
  case cs_or:
    NEED(stack_top, 2);
    stack_top--;
    if (arg_stack[stack_top] || arg_stack[stack_top-1]) {
      arg_stack[stack_top-1] = 1.0;
    } else {
      arg_stack[stack_top-1] = 0.0;
    }
    break;
  case cs_not:
    NEED(stack_top, 1);
    if (arg_stack[stack_top-1]) {
      arg_stack[stack_top-1] = 0.0;
    } else {
      arg_stack[stack_top-1] = 1.0;
    }
    break;
  case cs_abs:
    NEED(stack_top, 1);
    arg_stack[stack_top-1] = fabs(arg_stack[stack_top-1]);
    break;
  case cs_add:
    NEED(stack_top, 2);
    arg_stack[stack_top-2] += arg_stack[stack_top-1];
    stack_top--;
    break;
  case cs_sub:
    NEED(stack_top, 2);
    arg_stack[stack_top-2] -= arg_stack[stack_top-1];
    stack_top--;
    break;
  case cs_div: /* doesn't check overflow */
    NEED(stack_top, 2);
    arg_stack[stack_top-2] /= arg_stack[stack_top-1];
    stack_top--;
    break;
  case cs_neg:
    NEED(stack_top, 1);
    arg_stack[stack_top-1] *= -1.0;
    break;
  case cs_eq:
    NEED(stack_top, 2);
    stack_top--;
    if (arg_stack[stack_top] == arg_stack[stack_top-1]) {
      arg_stack[stack_top-1] = 1.0;
    } else {
      arg_stack[stack_top-1] = 0.0;
    }
    break;
  case cs_drop:
    NEED(stack_top, 1);
    stack_top--;
    break;
  case cs_put:
    NEED(stack_top, 2);
    {
      int idx = (int)arg_stack[--stack_top];
      NEED(CS_TRANS_ARRAY_MAX, idx);
      trn_array[idx] = arg_stack[--stack_top];
    }
    break;
  case cs_get:
    NEED(stack_top, 1);
    {
      int idx = (int)arg_stack[stack_top-1];
      NEED(CS_TRANS_ARRAY_MAX, idx);
      arg_stack[stack_top-1] = trn_array[idx];
    }
    break;
  case cs_ifelse:
    NEED(stack_top, 4);
    stack_top -= 3;
    if (arg_stack[stack_top+1] > arg_stack[stack_top+2]) {
      arg_stack[stack_top-1] = arg_stack[stack_top];
    }
    break;
  case cs_mul:
    NEED(stack_top, 2);
    arg_stack[stack_top-2] = arg_stack[stack_top-2] * arg_stack[stack_top-1];
    stack_top--;
    break;
  case cs_sqrt:
    NEED(stack_top, 1);
    arg_stack[stack_top-1] = sqrt(arg_stack[stack_top-1]);
    break;
  case cs_dup:
    NEED(stack_top, 1);
    NEED(CS_ARG_STACK_MAX, stack_top+1);
    arg_stack[stack_top] = arg_stack[stack_top-1];
    stack_top++;
    break;
  case cs_exch:
    NEED(stack_top, 2);
    {
      double save = arg_stack[stack_top-2];
      arg_stack[stack_top-2] = arg_stack[stack_top-1];
      arg_stack[stack_top-1] = save;
    }
    break;
  case cs_index:
    NEED(stack_top, 2); /* need two arguments at least */
    {
      int idx = (int)arg_stack[stack_top-1];
      if (idx < 0) {
	arg_stack[stack_top-1] = arg_stack[stack_top-2];
      } else {
	NEED(stack_top, idx+2);
	arg_stack[stack_top-1] = arg_stack[stack_top-idx-2];
      }
    }
    break;
  case cs_roll:
    NEED(stack_top, 2);
    {
      int N, J;
      J = (int)arg_stack[--stack_top];
      N = (int)arg_stack[--stack_top];
      NEED(stack_top, N);
      if (J > 0) {
	J = J % N;
	while (J-- > 0) {
	  double save = arg_stack[stack_top-1];
	  int i = stack_top - 1;
	  while (i > stack_top-N) {
	    arg_stack[i] = arg_stack[i-1];
	    i--;
	  }
	  arg_stack[i] = save;
	}
      } else {
	J = (-J) % N;
	while (J-- > 0) {
	  double save = arg_stack[stack_top-N];
	  int i = stack_top - N;
	  while (i < stack_top-1) {
	    arg_stack[i] = arg_stack[i+1];
	    i++;
	  }
	  arg_stack[i] = save;
	}
      }
    }
    break;
  case cs_random:
    fprintf(stderr, "\n*** Warning: Charstring operator `random' found ***\n");
    NEED(CS_ARG_STACK_MAX, stack_top+1);
    arg_stack[stack_top++] = 1.0;
    break;
  default:
    /* no-op ? */
    fprintf(stderr, "\n*** Unknown charstring operator: <0c%02x> ***\n", op);
    status = CS_PARSE_ERROR;
    break;
  }

  return;
}

/*
  integer:
   exactly same as the DICT encoding (except 29)
*/
static void get_integer (card8 **data, card8 *endptr)
{
  long result = 0;
  card8 b0 = **data, b1, b2;

  *data += 1;

  if (b0 == 28) { /* shortint */
    SRC_NEED(endptr, *data + 2);
    b1 = **data;
    b2 = *(*data+1);
    result = b1*256+b2;
    if (result > 0x7fff)
      result -= 0x10000L;
    *data += 2;
  } else if (b0 >= 32 && b0 <= 246) { /* int (1) */
    result = b0 - 139;
  } else if (b0 >= 247 && b0 <= 250) { /* int (2) */
    SRC_NEED(endptr, *data + 1);
    b1 = **data;
    result = (b0-247)*256+b1+108;
    *data += 1;
  } else if (b0 >= 251 && b0 <= 254) {
    SRC_NEED(endptr, *data + 1);
    b1 = **data;
    result = -(b0-251)*256-b1-108;
    *data += 1;
  } else {
    status = CS_PARSE_ERROR;
    return;
  }

  NEED(CS_ARG_STACK_MAX, stack_top+1);
  arg_stack[stack_top++] = (double) result;

  return;
}

/* signed 16.16-bits fixed number */
static void get_fixed (card8 **data, card8 *endptr)
{
  long ivalue;
  double rvalue;

  *data += 1;

  SRC_NEED(endptr, *data + 4);

  ivalue = *(*data) * 0x100 + *(*data+1);
  rvalue = (ivalue > 0x7fffL) ? (ivalue - 0x10000L) : ivalue;
  ivalue = *(*data+2) * 0x100 + *(*data+3);
  rvalue += ((double) ivalue) / 0x10000L;

  NEED(CS_ARG_STACK_MAX, stack_top+1);
  arg_stack[stack_top++] = rvalue;
  *data += 4;

  return;
}

/*
  Subroutines:
   The bias for subroutine number is introduced in type 2 charstrings.

   subr:     set to a pointer to the subroutine charstring.
   len:      set to the length of subroutine charstring.
   subr_idx: CFF INDEX data that contains subroutines.
   id:       biased subroutine number.
*/
static void get_subr(card8 **subr, long *len, cff_index *subr_idx, long id)
{
  card16 count;

  if (subr_idx == NULL)
    ERROR("in get_subr(): No subroutine data available");

  count = subr_idx->count;

  /* Adding bias number */
  if (count < 1024) {
    id += 107;
  } else if (count < 33900) {
    id += 1131;
  } else {
    id += 32768;
  }

  if (id > count) {
    fprintf(stderr, "\nSubr index: %ld/%d\n", id, count);
    ERROR("Invalid Subr index.");
  }

  *len = (subr_idx->offset)[id + 1] - (subr_idx->offset)[id];
  *subr = subr_idx->data + (subr_idx->offset)[id] - 1;

  return;
}

/*
  NOTE:
   The Type 2 interpretation of a number encoded in five-bytes (those with
   an initial byte value of 255) differs from how it is interpreted in the
   Type 1 format.
*/

static void do_charstring (card8 **dest, card8 *limit,
			   card8 **data, card8 *endptr,
			   cff_index *gsubr_idx, cff_index *subr_idx
			   )
{
  card8 b0 = 0, *subr;
  long len;

  if (nest > CS_SUBR_NEST_MAX) {
    ERROR("in cs_do_charstring(): Subroutine nested too deeply.");
  }

  nest++;

  while (*data < endptr && status == CS_PARSE_OK) {
    b0 = **data;
    if (b0 == 255) { /* 16-bit.16-bit fixed signed number */
      get_fixed(data, endptr);
    } else if (b0 == cs_return) {
      status = CS_SUBR_RETURN;
    } else if (b0 == cs_callgsubr) {
      if (stack_top < 1) {
	status = CS_STACK_ERROR;
      } else {
	stack_top--;
	get_subr(&subr, &len, gsubr_idx, (long) arg_stack[stack_top]);
	if (*dest + len > limit)
	  ERROR("in do_charstring(): Possible buffer overflow");
	do_charstring(dest, limit, &subr, subr + len,
		      gsubr_idx, subr_idx);
	*data += 1;
      }
    } else if (b0 == cs_callsubr) {
      if (stack_top < 1) {
	status = CS_STACK_ERROR;
      } else {
	stack_top--;
	get_subr(&subr, &len, subr_idx, (long) arg_stack[stack_top]);
	if (limit < *dest + len)
	  ERROR("in do_charstring(): Possible buffer overflow");
	do_charstring(dest, limit, &subr, subr + len,
		      gsubr_idx, subr_idx);
	*data += 1;
      }
    } else if (b0 == cs_escape) {
      do_operator2(dest, limit, data, endptr);
    } else if (b0 < 32 && b0 != 28) { /* 19, 20 need mask */
      do_operator1(dest, limit, data, endptr);
    } else if ((b0 <= 22 && b0 >= 27) || b0 == 31) { /* reserved */
      status = CS_PARSE_ERROR; /* not an error ? */
    } else { /* integer */
      get_integer(data, endptr);
    }
  }

  if (status == CS_SUBR_RETURN) {
    status = CS_PARSE_OK;
  } else if (status == CS_CHAR_END && *data < endptr) {
    fprintf(stderr, "\n*** Warning: garbage after endchar ***\n");
  } else if (status < CS_PARSE_OK) { /* error */
    fprintf(stderr, "\n*** Parsing charstring failed ***\n");
    fprintf(stderr, "error: %d (stack depth: %d)\n", status, stack_top);
    ERROR("Cannot continue ...");
  }

  nest--;

  return;
}

/* Not just coping: expand all subroutines */
long cs_copy_charstring (card8 *dest, long destlen,
			 card8 *src, long srclen,
			 cff_index *gsubr, cff_index *subr)
{
  card8 *save = dest;

  /* initialization */
  status = CS_PARSE_OK;
  nest = 0;
  num_stems = 0;
  phase = 0;
  stack_top = 0;
  /* not used yet */
  width = 0.0;
  have_width = 0;

  /* expand call(g)subrs */
  do_charstring(&dest, dest+destlen, &src, src+srclen, gsubr, subr);

  return (long)(dest - save);
}
