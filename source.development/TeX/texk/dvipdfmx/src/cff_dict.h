/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/cff_dict.h,v 1.1 2004/01/08 13:44:13 rahtz Exp $
    
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

#ifndef _CFF_DICT_H_
#define _CFF_DICT_H_

#include "cff_types.h"
#include "cff.h"

#define CFF_CIDCOUNT_DEFAULT 8720

extern cff_dict *cff_new_dict (void);
extern void      cff_release_dict (cff_dict *dict);

extern void   cff_dict_set (cff_dict *dict, char *key, int idx, double value);
extern double cff_dict_get (cff_dict *dict, char *key, int idx);
extern void   cff_dict_add (cff_dict *dict, char *key, int count);
extern void   cff_dict_remove (cff_dict *dict, char *key);
extern int    cff_dict_known (cff_dict *dict, char *key);

/* decode/encode DICT */
extern cff_dict *cff_dict_unpack (card8 *data, card8 *endptr);
extern long      cff_dict_pack (cff_dict *dict, card8 *dest, long destlen);

#endif /* _CFF_DICT_H_ */
