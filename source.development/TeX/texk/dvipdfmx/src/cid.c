/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/cid.c,v 1.1 2004/01/08 13:44:13 rahtz Exp $
    
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

/*
  CID-keyed font support:
   See also, cidbase, cidtype0, and cidtype2
*/

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "system.h"
#include "mem.h"
#include "error.h"

#include "pdfobj.h"

#include "cmap.h"

#include "cidbase.h"
#include "cidtype0.h"
#include "cidtype2.h"
#include "cid.h"

static unsigned char verbose = 0;

void cid_set_verbose(void)
{
  if (verbose < 255) verbose++;

  cidbase_set_verbose();
  cidtype0_set_verbose();
  cidtype2_set_verbose();
}

/*
  Standard character collections

  PDF 1.3
   Adobe-GB1-2
   Adobe-CNS1-0
   Adobe-Japan1-2
   Adobe-Korea1-1

  PDF-1.4
   Adobe-GB1-4
   Adobe-CNS1-3
   Adobe-Japan1-4
   Adobe-Korea1-2
*/

#define MAX_FONTS 4u
static unsigned num_cids = 0, max_cids = 0;
static struct cid_font_s *cids = NULL;

/* add CIDSystemInfo to dict */
void cid_add_sysinfo(pdf_obj *dict, struct cid_sysinfo_s ros)
{
  pdf_obj *systeminfo;

  if (ros.registry == NULL || ros.ordering == NULL ||
      ros.supplement < 0) {
    ERROR("cid_add_system_info(): ROS not correctly specified");
  }

  systeminfo = pdf_new_dict ();
  pdf_add_dict (systeminfo,
		pdf_new_name ("Registry"),
		pdf_new_string (ros.registry, strlen(ros.registry)));
  pdf_add_dict (systeminfo,
		pdf_new_name ("Ordering"),
		pdf_new_string (ros.ordering, strlen(ros.ordering)));
  pdf_add_dict (systeminfo,
		pdf_new_name ("Supplement"),
		pdf_new_number (ros.supplement));

  pdf_add_dict (dict, pdf_new_name ("CIDSystemInfo"), systeminfo);
  /*
  pdf_release_obj(systeminfo);
  */

  return;
}

/* font type, see cid.h */
int cid_fonttype (int cid_id)
{
  if (cid_id >= 0 && cid_id < num_cids) {
    return cids[cid_id].type;
  } else {
    ERROR("cid_fonttype(): invalid id");
    return -1;
  }
}

/* will be embedded or not */
int cid_embedd (int cid_id)
{
  if (cid_id >= 0 && cid_id < num_cids) {
    return cids[cid_id].embedd;
  } else {
    ERROR("cid_embedd(): invalid id");
    return 0;
  }
}

struct cid_font_s *cid_this_font (int cid_id)
{
  struct cid_font_s *result;

  if (cid_id >= 0 && cid_id < num_cids) {
    result = &(cids[cid_id]);
  } else {
    ERROR("cid_this_font(): invalid id");
  }

  return result;
}

/* id of parent Type0 font, wmode: 0 for horizontal, 1 for vertical */
int cid_parent_id (int cid_id, int wmode)
{
  int parent_id;

  if (wmode < 0 || wmode > 1)
    ERROR("cid_parent_id(): invalid wmode");

  if (cid_id >= 0 && cid_id < num_cids) {
    if ((parent_id = (cids[cid_id].parent)[wmode]) < 0) {
      return -1; /* have no parent */
    }
  } else {
    ERROR("cid_parent_id(): invalid id");
  }

  return parent_id;
}

/* set parent ant returns indirect reference to a CID-keyed font */
pdf_obj *cid_descendant (int type0_id, int cid_id, int wmode)
{
  if (wmode < 0 || wmode > 1)
    ERROR("cid_descendant(): invalid wmode");

  if (cid_id >= 0 && cid_id < num_cids) {
    (cids[cid_id].parent)[wmode] = type0_id;
    return pdf_link_obj(cids[cid_id].indirect);
  } else {
    ERROR ("cid_descendant(): invalid cid font id");
    return NULL;
  }
}

/* new entry */
struct cid_font_s *cid_new_font (int type, struct cid_sysinfo_s *ros)
{
  struct cid_font_s *cidfont = NULL;

  if (num_cids >= max_cids) {
    max_cids += MAX_FONTS;
    cids = RENEW (cids, max_cids, struct cid_font_s);
  }

  cidfont = &(cids[num_cids]);
  cids[num_cids].fontname = NULL;
  cids[num_cids].indirect = NULL;
  cids[num_cids].fontdict = NULL;
  cids[num_cids].descriptor = NULL;
  cids[num_cids].fontfile = NULL;
  cids[num_cids].cidset = NULL;
  cids[num_cids].w_array = NULL;
  cids[num_cids].w2_array = NULL;
  cids[num_cids].ident = NULL;
  cids[num_cids].type = type;
  (cids[num_cids].parent)[0] = -1; /* horizontal */
  (cids[num_cids].parent)[1] = -1; /* vertical */
  /* CIDSystemInfo */
  if (ros && ros->registry) {
    cids[num_cids].sysinfo.registry = NEW(strlen(ros->registry)+1, char);
    strcpy(cids[num_cids].sysinfo.registry, ros->registry);
  } else {
    cids[num_cids].sysinfo.registry = NULL;
  }
  if (ros && ros->ordering) {
    cids[num_cids].sysinfo.ordering = NEW(strlen(ros->ordering)+1, char);
    strcpy(cids[num_cids].sysinfo.ordering, ros->ordering);
  } else {
    cids[num_cids].sysinfo.ordering = NULL;
  }
  if (ros) {
    cids[num_cids].sysinfo.supplement = ros->supplement;
  } else {
    cids[num_cids].sysinfo.supplement = 0;
  }

  /* */
  cids[num_cids].embedd = 0;
  cids[num_cids].index = 0;
  cids[num_cids].filename = NULL;
  num_cids += 1;

  return cidfont;
}

/* returns PostScript name */
char *cid_fontname (int cid_id)
{
  if (cid_id >= 0 && cid_id < num_cids) {
    return cids[cid_id].fontname;
  } else {
    ERROR("cid_fontname(): invalid id");
    return NULL;
  }
}

/*
  If CMap's ROS is not compatible with already found font,
  then try to find another one.
*/

#define COMPAIR(p1, p2, q1, q2) (p1 && p2 && q1 && q2 && \
  strcmp(p1, q1) == 0 && strcmp(p2, q2) == 0)

int cid_get_id (const char *map_name, int cmap_id)
{
  int result = -1;
  struct cid_sysinfo_s *sysinfo = NULL;
  int i;

  for (i=0; i<num_cids; i++) {
    if (cids[i].ident && !strcmp (cids[i].ident, map_name)) {
      /* check character collection also */
      sysinfo = cmap_sysinfo(cmap_id);
      if (COMPAIR(cids[i].sysinfo.registry, cids[i].sysinfo.ordering,
		  sysinfo->registry, sysinfo->ordering)) {
	/* supplement may be increased */
	result = i;
	break;
      }
    }
  }

  if (i < num_cids) {
    if (cids[i].sysinfo.supplement < sysinfo->supplement) {
      if (cids[i].type == CIDFONT_TYPE2) {
	cids[i].sysinfo.supplement = sysinfo->supplement;
      } else {
	fprintf(stderr, "\n*** Warning: CMap has higher supplement ***\n");
      }
    }
  } else {
    /*
      xxx_get_id calls cid_new_font() which increases num_cids on success.
      cid id of newly found CID-keyed font is now num_cids - 1.
    */
    if (cidbase_get_id(map_name, cmap_id) < 0 &&
	cidtype0_get_id(map_name, cmap_id) < 0
	&& cidtype2_get_id(map_name, cmap_id) < 0
	) { /* all xxx_get_id failed */
      result = -1;
    } else {
      result = num_cids - 1; /* last cid id */
    }
  }

  return result;
}

static void do_cid (int id)
{
  switch (cids[id].type) {
  case CIDFONT_BASE:
    if(verbose)
      fprintf(stderr, "[predefined]:");
    do_cidbase(id);
    break;
  case CIDFONT_TYPE0:
    if(verbose)
      fprintf(stderr, "[CIDFontType0]:");
    do_cidtype0(id);
    break;
  case CIDFONT_TYPE2:
    if(verbose)
      fprintf(stderr, "[CIDFontType2]:");
    do_cidtype2(id);
    break;
  default:
    ERROR("[unknown type]");
  }
}

static void cid_release (int id)
{
  if (id >= 0 && id < num_cids) {
    switch(cids[id].type) {
    case CIDFONT_BASE:
      cidbase_release(id);
      break;
    case CIDFONT_TYPE0:
      cidtype0_release(id);
      break;
    case CIDFONT_TYPE2:
      cidtype2_release(id);
      break;
    default:
      ERROR("CID font: unknown type");
    }
    /* PDF objects */
    if (cids[id].indirect) pdf_release_obj (cids[id].indirect);
    if (cids[id].fontdict) pdf_release_obj (cids[id].fontdict);
    if (cids[id].descriptor) pdf_release_obj (cids[id].descriptor);
    if (cids[id].fontfile) pdf_release_obj (cids[id].fontfile);
    if (cids[id].cidset) pdf_release_obj (cids[id].cidset);
    if (cids[id].w_array) pdf_release_obj (cids[id].w_array);
    if (cids[id].w2_array) pdf_release_obj (cids[id].w2_array);
    /* --- */
    if (cids[id].fontname) RELEASE (cids[id].fontname);
    if (cids[id].ident) RELEASE (cids[id].ident);
    if (cids[id].filename) RELEASE (cids[id].filename);
    if (cids[id].sysinfo.registry)
      RELEASE (cids[id].sysinfo.registry);
    if (cids[id].sysinfo.ordering)
      RELEASE (cids[id].sysinfo.ordering);
  } else {
    ERROR("cid_release: passed invalid id");
  }
}

void cid_flush_all (void)
{
  int i;

  for (i=0; i<num_cids; i++) {
    if (verbose)
      fprintf(stderr, "(CID");
    do_cid(i);
    cid_release(i);
    if (verbose)
      fprintf(stderr, ")");
  }

  if (cids)
    RELEASE(cids);
}

/* also in type1.c */
void mangle_fontname(char *fontname)
{
  int i;
  char ch;
  static char first = 1;
  memmove (fontname+7, fontname, strlen(fontname)+1);
  /* The following procedure isn't very random, but it
     doesn't need to be for this application. */
  if (first) {
    srand (time(NULL));
    first = 0;
  }
  for (i=0; i<6; i++) {
    ch = rand() % 26;
    fontname[i] = ch+'A';
  }
  fontname[6] = '+';
}

/*
   OpenType font should be specified in the form
     [!]fontfile[.otf][,variant]
   in fontmap file. Note that fontfile must not start with
   the character ':'.

   TrueType collection font should be specified in the form
     :index:[!]fontfile[.ttc][,variant]
   in fonamap file.

   TrueType font should be specified in the form
     :0:[!]fontfile[.ttf][,variant]
   in fonamap file. It is used to distinguish CJK TrueType fonts
   and other fonts.
*/
int split_face(char *str, char **attrib, int *embedd)
{
  char *second, *third;
  int face, len;

  if (str[0] == ':') { /* CIDFontType2 */
    face = strtol(str+1, &second, 10);
    if (str+1 == second || second[0] != ':' || second[1] == '\0')
      return -1;
    second++;
  } else if (str[0] == ' ' && str[1] != ':') { /* not CIDFontType0 */
    face = 0;
    second = str + 1;
  } else
    return -1;

  if (*second == '!') { /* no-embedding */
    *embedd = 0;
    if (*(++second) == '\0')
      return -1;
  }

  *attrib = NULL;

  if ((third = strchr(second, ',')) != NULL) { /* with stylistic variant */
    *third++ = '\0';
    if (strcmp(third, "Bold") && strcmp(third, "Italic") &&
        strcmp(third, "BoldItalic"))
      fprintf(stderr, "\n*** Warning: Invalid variant '%s' in '%s'\n             Supports only Bold, Italic, and BoldItalic.\n", third, second);
    else {
      *attrib = third - 1;
      if (*embedd != 0) {
        fprintf(stderr, "\n*** Warning: Embedding is specified in '%s' with the variant '%s'.\n             Ignores the embedding feature.\n", second, third);
        *embedd = 0;
      }
    }
  }

  memmove(str, second, strlen(second)+1);

  if (*attrib) **attrib = ',';

  return face;
}
