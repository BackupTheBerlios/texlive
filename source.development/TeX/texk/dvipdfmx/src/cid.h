/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/cid.h,v 1.1 2004/01/08 13:44:13 rahtz Exp $
    
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

#ifndef _CID_H_
#define _CID_H_

#include "pdfobj.h"

/* Limits */
#define CID_MAX_CID  65535
#define CID_MAX_CODE 65535

/* .notdef CID */
#define CID_NOTDEF   0

/* CIDFont types */
#define CIDFONT_BASE  0
#define CIDFONT_TYPE0 1
#define CIDFONT_TYPE2 2

struct cid_sysinfo_s
{
  char *registry;
  char *ordering;
  int   supplement;
};

struct cid_font_s
{
  char    *fontname;   /* PostScript font name */
  int      type;       /* CIDFONT_BASE or CIDFONT_TYPE0 or CIDFONT_TYPE2 */
  int      parent[2];  /* parent type0 font of this CID-keyed font: h, v */
  struct cid_sysinfo_s sysinfo; /* character collection */
  /* - PDF resource - */
  pdf_obj *indirect;   /* indirect reference to CIDFont dictionary */
  pdf_obj *fontdict;   /* CIDFont dictionary */
  pdf_obj *descriptor; /* FontDescriptor */
  pdf_obj *fontfile;   /* FontFile stream */
  pdf_obj *cidset;     /* CIDSet stream */
  pdf_obj *w_array;    /* W array, for future use */
  pdf_obj *w2_array;   /* W2 array, for future use */
  /* - - */
  char   *ident;       /* this is actually mapname */
  char   *filename;    /* this is usually mapname */
  int     index;       /* TTC index, basefont id, CFF fontset id (?) */
  int     embedd;      /* 1 for embedded font */
};

extern void cid_set_verbose(void);

extern int  cid_get_id (const char *map_name, int cmap_id);
extern void cid_flush_all (void);

extern struct cid_font_s *cid_new_font (int type, struct cid_sysinfo_s *ros);
extern struct cid_font_s *cid_this_font (int cid_id);

extern char *cid_fontname (int cid_id);
extern int   cid_parent_id (int cid_id, int wmode);
extern int   cid_fonttype (int cid_id);
extern int   cid_embedd (int cid_id);

extern void     cid_add_sysinfo (pdf_obj *dict, struct cid_sysinfo_s ros);
extern pdf_obj *cid_descendant (int type0_id, int cid_id, int wmode);

extern void mangle_fontname(char *fontname);
extern int split_face(char *str, char **attrib, int *embedd);

#endif /* _CID_H_ */
