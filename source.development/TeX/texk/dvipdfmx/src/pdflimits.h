/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/pdflimits.h,v 1.1 2004/01/08 13:44:14 rahtz Exp $

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

#ifndef _PDFLIMITS_H_
#define _PDFLIMITS_H_

#define MAX_FONTS 4u /* Font group allocation size, not a hard maximum */
#define DVI_MAX_STACK_DEPTH 256u
#define MAX_VF_NESTING 16u
#define MAX_COLORS 32u
#define MAX_TRANSFORMS 32u
#define MAX_OUTLINE_DEPTH 32u
#define MAX_ARTICLES 64u
#define MAX_ENCODINGS 8u
#define IND_OBJECTS_ALLOC_SIZE 1024u
#define PAGES_ALLOC_SIZE 128u
#define DESTS_ALLOC_SIZE 1024u
#define NAMED_OBJ_ALLOC_SIZE 128u
#define VF_ALLOC_SIZE 16u
#define STREAM_ALLOC_SIZE 4096u
#define ARRAY_ALLOC_SIZE 256

#endif /* _PDFLIMITS_H_ */
