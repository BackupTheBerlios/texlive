/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/ps_parse.h,v 1.1 2004/01/08 13:44:15 rahtz Exp $
    
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

#ifndef _PS_PARSE_H_
#define _PS_PARSE_H_

#include <stdio.h>

/* typedef int ps_parse_status_t */
#define PS_PARSE_ERROR_MIN -2
#define PS_PARSE_BUFF_ERROR -2
#define PS_PARSE_INVALID -1
#define PS_PARSE_END 0
#define PS_PARSE_OK 1
#define PS_PARSE_STATUS_MAX 1

/* typedef int ps_token_type_t */
#define PS_UNKNOWN (1 << 0)
#define PS_TOKEN (1 << 1)
#define PS_NAME (1 << 2)
#define PS_STRING (1 << 3)
#define PS_STRING_AHX (1 << 4)
#define PS_STRING_A85 (1 << 5)
#define PS_INTEGER (1 << 6)
#define PS_REAL (1 << 7)
#define PS_RADIX (1 << 8)
#define PS_BEGIN_ARRAY (1 << 9)
#define PS_END_ARRAY (1 << 10)
#define PS_BEGIN_PROC (1 << 11)
#define PS_END_PROC (1 << 12)
#define PS_BEGIN_DICT (1 << 13)
#define PS_END_DICT (1 << 14)

extern void ps_parse_set_verbose (void);
extern int ps_parse_read_line (FILE *stream, char *buffer, int bufflen);
extern int ps_parse_get_token (FILE *stream, int *type,
			       char *dest, int destlen);

#endif /*  _PS_PARSE_H_ */
