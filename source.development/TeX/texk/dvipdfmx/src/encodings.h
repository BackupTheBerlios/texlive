/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/encodings.h,v 1.1 2004/01/08 13:44:14 rahtz Exp $
    
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

#ifndef _ENCODINGS_H_
#define _ENCODINGS_H_

extern void encoding_set_verbose(void);
extern pdf_obj *find_encoding_differences (pdf_obj *encoding);
extern pdf_obj *make_differences_encoding (pdf_obj *encoding);
extern int get_encoding (const char *enc_name);
extern void encoding_flush_all (void);
extern char *encoding_glyph (int encoding_id, unsigned code);
extern pdf_obj *encoding_ref (int encoding_id);

#endif /* _ENCODINGS_H_ */
