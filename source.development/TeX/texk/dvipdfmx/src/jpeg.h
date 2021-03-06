/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/jpeg.h,v 1.1 2004/01/08 13:44:14 rahtz Exp $

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

#ifndef _JPEG_H_
#define _JPEG_H_

#include <stdio.h>
#include "pdfobj.h"
#include "pdfspecial.h"

struct jpeg 
{
  unsigned width, height;
  unsigned bits_per_color;
  FILE *file;
  unsigned colors;
};

extern void jpeg_set_verbose(void);
extern struct jpeg *jpeg_open (FILE *file);
extern int jpeg_headers (struct jpeg *jpeg);
extern void jpeg_close (struct jpeg *jpeg);
extern int check_for_jpeg (FILE *file);

#endif /* _JPEG_H_ */
