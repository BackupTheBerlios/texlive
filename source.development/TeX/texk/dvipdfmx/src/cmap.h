/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/cmap.h,v 1.1 2004/01/08 13:44:13 rahtz Exp $
    
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

#ifndef _CMAP_H_
#define _CMAP_H_

/* CID_MAX_CID, CID_MAX_CODE, CID_NOTDEF, struct cid_sysinfo_s */
#include "cid.h"

/*
  CMap table lookup flags:
   CMAP_NO_INVALID
    treat as error when given character code is not in valid range.
   CMAP_NO_MISSING
    treat as error when given character code has no corresponding CID.
   CMAP_NO_WARN_INVALID/CMAP_NO_WARN_MISSING
    supress warning messages.
*/

#define CMAP_DEBUG           (1 << 0)
#define CMAP_NO_INVALID      (1 << 1)
#define CMAP_NO_MISSING      (1 << 2)
#define CMAP_NO_WARN_INVALID (1 << 3)
#define CMAP_NO_WARN_MISSING (1 << 4)

#define CMAP_IDENTITY_H 0
#define CMAP_IDENTITY_V 1

extern void cmap_set_lookup (int flag); /* set flag */

/* be verbose */
extern void cmap_set_verbose (void);

/* read or release CMap tables */
extern int  get_cmap (const char *cmap_name);
extern void cmap_close_all (void);

/* accessing CMap info */
extern char *cmap_cmapname (int cmap_id);
extern int   cmap_wmode (int cmap_id);
extern struct cid_sysinfo_s *cmap_sysinfo (int cmap_id);

/* lookup character mapping table */
extern unsigned short cmap_lookup (unsigned char hi, unsigned char lo,
				   int cmap_id);

#endif /* _CMAP_H_ */
