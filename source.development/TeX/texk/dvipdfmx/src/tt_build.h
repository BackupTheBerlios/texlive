/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/tt_build.h,v 1.1 2004/01/08 13:44:15 rahtz Exp $
    
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

#ifndef _TT_BUILD_H_
#define _TT_BUILD_H_

/*
  glyph with id = gid is mapped to new id = newgid.
  width for Widths and W (CIDFont) array.
  need bbox info. for W2 array build.
*/
struct glyph_desc
{
  USHORT gid;
  USHORT newgid;
  ULONG  offset;
  USHORT width;
};

struct tt_glyphs
{
  USHORT num_glyphs;
  USHORT max_glyphs;
  USHORT last_gid;
  USHORT emsize;
  struct glyph_desc *used_glyphs;
};

extern struct tt_glyphs *tt_build_init (void);
extern void tt_build_finish (struct tt_glyphs *g);

extern void tt_add_to_used (struct tt_glyphs *g, USHORT gid, USHORT new_gid);
extern int tt_build_tables (sfnt *sfont, struct tt_glyphs *g);

#endif /* _TT_BUILD_H_ */
