/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfm/psimage.h,v 1.1 2004/01/08 13:44:09 rahtz Exp $

    This is dvipdfm, a DVI to PDF translator.
    Copyright (C) 1998, 1999 by Mark A. Wicks

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    The author may be contacted via the e-mail address

	mwicks@kettering.edu
*/

#ifndef PSIMAGE_H
#define PSIMAGE_H

#include "pdfobj.h"
#include "pdfspecial.h"

extern void set_distiller_template (char *s);
extern pdf_obj *ps_include (char *file_name, 
			   struct xform_info *p,
			   char *res_name,
			   double x_user, double y_user);

extern int check_for_ps (FILE *image_file);
extern void psimage_close (void);
#endif
