/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/cidbase.c,v 1.1 2004/01/08 13:44:13 rahtz Exp $
    
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
  PDF base/core CID-keyed font:

   Ther is no standard PostScript font for CJK-font. Basically, all CJK
   fonts must be embedded in PDF file if you want to view/distribute
   PDF document that contains CJK characters. However, most PDF viewers
   are capable of automatically substituting CJK fonts that no glyph data
   available from PDF file with appropriate one available on the system.
   You can create PDF document which contains CJK characters without
   having any CJK fonts. You must have font to view documents, though.

   Fonts listed in cid_basefont.h can be used. Some information are
   required for each font to operate properly.
*/

#ifdef WIN32
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdio.h>
#include "system.h"
#include "mem.h"
#include "error.h"
#include "pdfobj.h"
#include "pdfparse.h"
#include "cid.h"
#include "cmap.h"
#include "cid_basefont.h"
#include "cidbase.h"

static unsigned char verbose = 0;

void cidbase_set_verbose(void)
{
  if (verbose < 255) verbose++;
}

static void cidbase_start_font_descriptor (struct cid_font_s *cidfont)
{
  int idx;
  pdf_obj *tmp;
  char *start, *end;

  idx = cidfont->index;
  cidfont->descriptor = pdf_new_dict();

  pdf_add_dict (cidfont->descriptor,
		pdf_new_name ("Type"),
		pdf_new_name ("FontDescriptor"));
  pdf_add_dict (cidfont->descriptor,
		pdf_new_name ("CapHeight"),
		pdf_new_number (cid_basefont[idx].capheight));
  pdf_add_dict (cidfont->descriptor,
		pdf_new_name ("Ascent"),
		pdf_new_number (cid_basefont[idx].ascent));
  pdf_add_dict (cidfont->descriptor,
		pdf_new_name ("Descent"),
		pdf_new_number (cid_basefont[idx].descent));

  start = (char *) cid_basefont[idx].bbox;
  end = start+strlen(cid_basefont[idx].bbox);
  tmp = parse_pdf_array (&start, end);
  pdf_add_dict (cidfont->descriptor, pdf_new_name ("FontBBox"), tmp);

  pdf_add_dict (cidfont->descriptor,
		pdf_new_name ("FontName"),
		pdf_new_name (cid_basefont[idx].fontname));

  pdf_add_dict (cidfont->descriptor,
		pdf_new_name ("ItalicAngle"),
		pdf_new_number(cid_basefont[idx].italic));
  pdf_add_dict (cidfont->descriptor,
		pdf_new_name ("StemV"),
		pdf_new_number(cid_basefont[idx].stemv));
  pdf_add_dict (cidfont->descriptor,
		pdf_new_name ("Flags"),
		pdf_new_number(cid_basefont[idx].flags));

  /* optional entries */
  if (cid_basefont[idx].xheight) {
    pdf_add_dict (cidfont->descriptor,
		  pdf_new_name ("XHeight"),
		  pdf_new_number(cid_basefont[idx].xheight));
  }
  if (cid_basefont[idx].avgw) {
    pdf_add_dict (cidfont->descriptor,
		  pdf_new_name ("AvgWidth"),
		  pdf_new_number(cid_basefont[idx].avgw));
  }
  if (cid_basefont[idx].leading) {
    pdf_add_dict (cidfont->descriptor,
		  pdf_new_name ("Leading"),
		  pdf_new_number(cid_basefont[idx].leading));
  }
  if (cid_basefont[idx].maxw) {
    pdf_add_dict (cidfont->descriptor,
		  pdf_new_name ("MaxWidth"),
		  pdf_new_number(cid_basefont[idx].maxw));
  }
  if (cid_basefont[idx].missw) {
    pdf_add_dict (cidfont->descriptor,
		  pdf_new_name ("MissingWidth"),
		  pdf_new_number(cid_basefont[idx].missw));
  }
  if (cid_basefont[idx].stemh) {
    pdf_add_dict (cidfont->descriptor,
		  pdf_new_name ("StemH"),
		  pdf_new_number(cid_basefont[idx].stemh));
  }
  /* CharSet (?) */
  if (cid_basefont[idx].charset) {
    pdf_add_dict (cidfont->descriptor,
		  pdf_new_name ("CharSet"),
		  pdf_new_string (cid_basefont[idx].charset,
				  strlen(cid_basefont[idx].charset)));
  }
  /* Style: optional, but important one */
  if (cid_basefont[idx].style) {
    start = (char *) cid_basefont[idx].style;
    end = start+strlen(cid_basefont[idx].style);
    tmp = parse_pdf_object (&start, end);
    pdf_add_dict (cidfont->descriptor, pdf_new_name ("Style"), tmp);
  }

  return;
}

int cidbase_get_id (const char *map_name, int cmap_id)
{
  struct cid_font_s *cidfont;
  struct cid_sysinfo_s *sysinfo;
  int i, idx;
  pdf_obj *tmp;
  char *start, *end, *delim;

  if ((delim = strchr(map_name, ',')) != NULL)
    *delim = '\0';
  for (i=0;i < CID_LAST_BASEFONT;i++) {
    if (!strcmp(map_name, cid_basefont[i].fontname)) {
      break;
    }
  }
  if (delim) {
    delim++;
    if (!strcmp(delim, "Bold") || !strcmp(delim, "Italic") ||
        !strcmp(delim, "BoldItalic")) *(--delim) = ',';
    else {
      fprintf(stderr, "\n*** Warning: Invalid attribute %s,%s\n",
              map_name, delim);
      fprintf(stderr, "Supports only Bold, Italic, and BoldItalic.\n");
    }
  }
  if (i >= CID_LAST_BASEFONT)
    return -1; /* not a basefont */

  idx = i; /* basefont id */
  sysinfo = cmap_sysinfo(cmap_id);
  if (strcmp(sysinfo->registry, cid_basefont[idx].cc.registry) != 0 ||
      strcmp(sysinfo->ordering, cid_basefont[idx].cc.ordering) != 0
      ) {
    fprintf(stderr, "\n*** Unacceptable CMap ***\n");
    fprintf(stderr, "Fontname matched but character collection mismatched.\n");
    fprintf(stderr, "Incompatible CMap specified in mapfile ?\n");
    return -1;
  } else if (sysinfo->supplement > cid_basefont[idx].cc.supplement) {
    fprintf(stderr,
	    "\n*** Warning: CMap file have higher supplement number");
    fprintf(stderr, " than CID-keyed font ***\n");
  }

  cidfont = cid_new_font(CIDFONT_BASE,
			 (struct cid_sysinfo_s *) &(cid_basefont[idx].cc));

  cidfont->fontname = NEW (strlen(map_name)+1, char);
  strcpy (cidfont->fontname, map_name);
  cidfont->ident = NEW (strlen(map_name)+1, char);
  strcpy (cidfont->ident, map_name);
  cidfont->index = idx;

  cidfont->embedd = 0;
  cidfont->fontfile = NULL;
  cidfont->cidset = NULL;
  cidfont->w_array = NULL;

  cidfont->fontdict = pdf_new_dict ();
  cidfont->indirect = pdf_ref_obj(cidfont->fontdict);

  cidbase_start_font_descriptor(cidfont);

  pdf_add_dict (cidfont->fontdict,
		pdf_new_name ("Type"),
		pdf_new_name ("Font"));
  pdf_add_dict (cidfont->fontdict,
		pdf_new_name ("Subtype"),
		pdf_new_name (cid_basefont[idx].subtype));
  pdf_add_dict (cidfont->fontdict,
		pdf_new_name ("BaseFont"),
		pdf_new_name (cidfont->fontname));
  pdf_add_dict (cidfont->fontdict, 
		pdf_new_name ("FontDescriptor"),
		pdf_ref_obj(cidfont->descriptor));

  cid_add_sysinfo (cidfont->fontdict, cidfont->sysinfo);

  /* optional entries */
  if (cid_basefont[idx].dw > 0) {
    pdf_add_dict (cidfont->fontdict, 
		  pdf_new_name ("DW"),
		  pdf_new_number (cid_basefont[idx].dw));
  }
  if (cid_basefont[idx].w) {
    start = (char *) cid_basefont[idx].w;
    end = start+strlen(cid_basefont[idx].w);
    tmp = parse_pdf_array (&start, end);
    pdf_add_dict (cidfont->fontdict, pdf_new_name ("W"), tmp);
  }
  if (cid_basefont[idx].dw2) {
    start = (char *) cid_basefont[idx].dw2;
    end = start+strlen(cid_basefont[idx].dw2);
    tmp = parse_pdf_array (&start, end);
    pdf_add_dict (cidfont->fontdict, pdf_new_name ("DW2"), tmp);
  }
  if (cid_basefont[idx].w2) {
    start = (char *) cid_basefont[idx].w2;
    end = start+strlen(cid_basefont[idx].w2);
    tmp = parse_pdf_array (&start, end);
    pdf_add_dict (cidfont->fontdict, pdf_new_name ("W2"), tmp);
  }

  /*
    We are not embedding font, all neccesary information are already given.
    Flush and release the content.
  */

  pdf_release_obj (cidfont->fontdict);
  cidfont->fontdict = NULL;

  pdf_release_obj(cidfont->descriptor);
  cidfont->descriptor = NULL;

  return 0; /* found */
}

void do_cidbase (int id)
{
  /* nothing to be done */
}

void cidbase_release (int id)
{
  /* nothing */
}
