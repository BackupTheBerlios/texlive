/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/numbers.h,v 1.1 2004/01/08 13:44:14 rahtz Exp $

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

#ifndef _NUMBERS_H_
#define _NUMBERS_H_

#include <stdio.h>
#include <math.h>

typedef unsigned char Ubyte;
typedef int UNSIGNED_BYTE, SIGNED_BYTE, SIGNED_PAIR;
typedef unsigned UNSIGNED_PAIR;
typedef long UNSIGNED_TRIPLE, SIGNED_TRIPLE, SIGNED_QUAD;
typedef unsigned long UNSIGNED_QUAD;

extern UNSIGNED_BYTE get_unsigned_byte (FILE *);
extern UNSIGNED_BYTE sget_unsigned_byte (char *);
extern SIGNED_BYTE get_signed_byte (FILE *);
extern UNSIGNED_PAIR get_unsigned_pair (FILE *);
extern UNSIGNED_PAIR sget_unsigned_pair (unsigned char *);
extern SIGNED_PAIR get_signed_pair (FILE *);
extern UNSIGNED_TRIPLE get_unsigned_triple (FILE *);
extern SIGNED_TRIPLE get_signed_triple (FILE *);
extern SIGNED_QUAD get_signed_quad (FILE *);
extern UNSIGNED_QUAD get_unsigned_quad (FILE *);

typedef signed long fixword;

extern SIGNED_QUAD sqxfw (SIGNED_QUAD sq, fixword fw);
extern SIGNED_QUAD axboverc (SIGNED_QUAD n1, SIGNED_QUAD n2, SIGNED_QUAD div);

#define ROUND(n,acc) (floor(((double)n)/(acc)+0.5)*(acc)) 
#ifndef MAX
  #define MAX(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef MIN
  #define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#define ISODD(n) (((n)/2)*2!=(n))
#define ISEVEN(n) (((n)/2)*2==(n))

extern long one_bp;
extern long one_hundred_bp;
extern long min_bp_val;
extern long scaled_out;
extern int fixed_decimal_digits;

extern int pdf_print_bp (char *buf, long s);
extern int pdf_print_pt (char *buf, double s);
extern int pdf_print_int (char *buf, long s);
extern void init_pdf_numbers (int pdfdecimaldigits);

#endif /* _NUMBERS_H_ */
