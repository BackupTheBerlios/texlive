/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/colorsp.h,v 1.1 2004/01/08 13:44:13 rahtz Exp $
    
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

#ifndef _COLORSP_H_
#define _COLORSP_H_

#include "pdfparse.h"

int color_special (char *buffer, UNSIGNED_QUAD size);
void color_special_ignore_colors(void);

#endif /* _COLORSP_H_ */
