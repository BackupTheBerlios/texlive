/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/cidtype2.c,v 1.1 2004/01/08 13:44:13 rahtz Exp $
    
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

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "system.h"
#include "numbers.h"
#include "mem.h"
#include "mfileio.h"
#include "error.h"

#include "pdfobj.h"

/* TrueType */
#include "sfnt.h"
#include "tt_aux.h"
#include "tt_build.h"
#include "tt_cmap.h"
#include "tt_gsub.h"
#include "tt_table.h"

/* CID font */
#include "cid.h"
#include "cmap.h"
#include "type0.h"

#include "cidtype2.h"

static unsigned char verbose = 0;

void cidtype2_set_verbose(void)
{
  if (verbose < 255) verbose++;
}

/*
  PDF_NAME_LEN_MAX: see, Appendix C of PDF Ref. v1.3, 2nd. ed.
  This is Acrobat implementation limit.
*/
#define PDF_NAME_LEN_MAX 127

/*
  PDF viewer applications use following tables (CIDFontType 2)

   head, hhea, loca, maxp, glyf, hmtx, fpgm, cvt_, prep

                                           - from PDF Ref. v.1.3, 2nd ed.

  The fpgm, cvt_, and prep tables appears only when TrueType instructions
  requires them. Those tables must be preserved if they exist.
  We use must_exist flag to indicate `preserve it if it is present'
  and to make sure not to cause an error when it does not exist.

  post and name table must exist in ordinary TrueType font file,
  but when a TrueType font is converted to CIDFontType 2 font, those tables
  are no longer required.

  The OS/2 table (required for TrueType font for Windows and OS/2) contains
  liscencing information, but PDF viewers seems not using them.

  2001/11/23: OS/2 table added to required tables list.

  TODO:
   Adobe says that vmtx is not used by PDF viewers. We must extract
   information corresponds to W2 and DW2 in font dictionary from vmtx and
   bounding box of each glyphs.
*/

static struct
{
  char *name;
  int must_exist;
} required_table[] = {
  {"OS/2", 1}, {"head", 1}, {"hhea", 1}, {"loca", 1}, {"maxp", 1},
  {"glyf", 1}, {"hmtx", 1}, {"fpgm", 0}, {"cvt ", 0}, {"prep", 0}
};

#define LAST_REQ_TABLE (sizeof(required_table)/sizeof(*required_table))

/*
  in PDF 1.2 or higher
   All characters not printable in ASCII and some characters that have
   specail meaning in PDF must be converted to #xx form in name string,
   where xx is a hexadecimal notion of a character code.
   The null character (0) is not allowed.

  in PDF 1.1
   Remove all characters which can not be used in name string.

  The new name strings does not contains '\0' in the middle of them.

  TODO:
   We must know current setting of PDF version.
*/
static void validate_name (char **name, int len)
{
  char *tmp;
  int i, pos;

  if (len > PDF_NAME_LEN_MAX)
    ERROR("Name string length too large");

  /* eg, 0x5b -> #5b */
  tmp = NEW(3*len + 1, char);

  pos = 0;
  for (i=0;i<len;i++) {
    if (*(*name+i) == 0) {
      /* skip */
    } else if (*(*name+i) < '!' || *(*name+i) > '~' ||
	       /*      ^ `space' is here */
	       strchr("/()[]<>{}#", *(*name+i)) != NULL) {
      sprintf(tmp+pos, "#%02x", (unsigned char) *(*name+i));
      pos += 3;
    } else {
      *(tmp+pos) = *(*name+i);
      pos++;
    }
  }

  if (pos != len) {
    *name = RENEW(*name, pos+1, char);
  }
  memmove(*name, tmp, pos);
  *(*name+pos) = '\0';

  RELEASE(tmp);

  /* For some fonts that have bad PS name. ad hoc. remove me. */
  if ((tmp = strstr(*name, "-WIN-RKSJ-H")) != NULL &&
      (tmp != *name)) {
    *tmp = '\0';
  } else if ((tmp = strstr(*name, "-WINP-RKSJ-H")) != NULL &&
	     (tmp != *name)) {
    *tmp = '\0';
  } else if ((tmp = strstr(*name, "-WING-RKSJ-H")) != NULL &&
	     (tmp != *name)) {
    *tmp = '\0';
  } else if ((tmp = strstr(*name, "-90pv-RKSJ-H")) != NULL &&
	     (tmp != *name)) {
    *tmp = '\0';
  }

  if (strlen(*name) == 0) {
    ERROR("No valid character found in name string.");
  }
}

/*
  Find ToCode CMap corresponding to given ROS and platform-encoding

   ToUnicode CMap (?) distributed with Adobe Acrobat Reader
    Registry-Ordering-UCS2

   In the ToUnicode tutorial, the ToUnicode CMap filename is in the
   following form:
    Registry-Ordering-ddd
   however, I'll not support that.

  I'll put following code there temporary. CJK-TrueType config file will
  be introduced in the future.

   [ident] [ROS] [TrueType cmap ID] [ToCodeCMap] [OT Layout Feature]
*/

/*  Johab unknown, KSC-Johab ? */
static struct
{
  unsigned short platform;
  unsigned short encoding;
  char *append;
} known_encodings[] = {
  {TT_WIN, TT_WIN_UNICODE, "UCS2"},                /* -,   UNI  (WIN) */
  {TT_WIN, TT_WIN_SJIS, "90ms-RKSJ"},              /* JPN, SJIS (WIN) */
  {TT_MAC, TT_MAC_JAPANESE, "90pv-RKSJ"},          /* JPN, SJIS (MAC) */
  {TT_WIN, TT_WIN_RPC, "GBK-EUC"},                 /* GB,  RPC  (WIN) */ 
  {TT_MAC, TT_MAC_SIMPLIFIED_CHINESE, "GBpc-EUC"}, /* GB,  RPC  (MAC) */ 
  {TT_WIN, TT_WIN_BIG5, "ETen-B5"},                /* CNS, BIG5 (WIN) */
  {TT_MAC, TT_MAC_TRADITIONAL_CHINESE, "B5pc"},    /* CNS, BIG5 (MAC) */
  {TT_WIN, TT_WIN_WANSUNG, "KSCms-UHC"},           /* KRN, ?    (WIN) */
  {TT_MAC, TT_MAC_KOREAN, "KSCpc-EUC"},            /* KRN, ?    (MAC) */
};
#define LAST_KNOWN_ENCODINGS (sizeof(known_encodings)/sizeof(*known_encodings))

static int find_tocode_cmap (char *reg, char *ord, char *append)
{
  int id = -1;
  int len;
  char *str;

  if ((reg == NULL) || (ord == NULL) || (append == NULL))
    return -1; /* invalid */

  if (!strcmp(ord, "Identity"))
    return get_cmap("Identity-H");

  len = strlen(reg) + strlen(ord) + strlen(append) + 2;

  if (len > PDF_NAME_LEN_MAX)
    ERROR("name string length too large");

  str = NEW(len + 1, char);
  sprintf(str, "%s-%s-%s", reg, ord, append);
  id = get_cmap(str); /* id = -1 for not found */
  RELEASE(str);

  return id;
}

static void do_w_array (pdf_obj *w_array, struct tt_glyphs *g,
			USHORT dw, unsigned short last_cid)
{
  unsigned short cid, start, prev, i;
  pdf_obj *an_array;
  double width;

  prev = 0;
  start = 0;
  an_array = NULL;
  for (i=0;i<(g->num_glyphs);i++) {
    if ((cid = (g->used_glyphs)[i].newgid) > last_cid) {
      break;
    }
    width = (g->used_glyphs)[i].width;
    if (width == dw) {
      if (an_array) {
	pdf_add_array(w_array, pdf_new_number(start));
	pdf_add_array(w_array, an_array);
	an_array = NULL;
      }
    } else {
      if (cid != prev + 1) {
	if (an_array) {
	  pdf_add_array(w_array, pdf_new_number(start));
	  pdf_add_array(w_array, an_array);
	  an_array = NULL;
	}
      }
      if (an_array == NULL) {
	an_array = pdf_new_array();
	start = cid;
      }
      width = ROUND(1000.0*(width)/(g->emsize), 1);
      pdf_add_array(an_array, pdf_new_number(width));
      prev = cid;
    }
  }

  if (an_array) {
    pdf_add_array(w_array, pdf_new_number(start));
    pdf_add_array(w_array, an_array);
  }

  /*
    W array may not contain any objects. In this case, the value of W array
    becomes null object, then the entry W is omitted as described in PDF
    reference v.1.3, section 3.2.8..
  */

}

void do_cidtype2 (int cid_id)
{
  char *result;
  char *shortname, *fullname;
  struct cid_font_s *cidfont;
  pdf_obj *stream_dict;
  sfnt *sfont;
  char *h_used_chars, *v_used_chars, *used_chars;
  struct tt_glyphs *glyphs;
  int parent_id, cmap_id, face = 0;
  tt_cmap *ttcmap = NULL;
  ULONG offset, length = 0;
  unsigned short cid, last_cid = 0;
  int i;

#ifdef MEM_DEBUG
MEM_START
#endif /* MEM_DEBUG */
  
  cidfont = cid_this_font(cid_id);

  cid_add_sysinfo(cidfont->fontdict, cidfont->sysinfo);

  if (cidfont->embedd == 0) {/* no embedd */
    pdf_release_obj(cidfont->fontdict);
    pdf_release_obj(cidfont->descriptor);
    cidfont->descriptor = NULL;
    cidfont->fontdict = NULL;
    return;
  }

  face = cidfont->index;
  shortname = cidfont->filename;

  fullname = kpse_find_file(shortname, kpse_truetype_format, 1);
  if (fullname == NULL) {
    fprintf (stderr, "Unable to find or open TTF file (%s)", shortname);
    ERROR ("This existed when I checked it earlier!");
  }

  if (verbose == 1) {
    fprintf(stderr, "%s", cidfont->ident);
  } else if (verbose > 1) {
    fprintf(stderr, "%s", fullname);
  }

  if ((sfont = sfnt_open(fullname)) == NULL)
    ERROR("cannot open TTF file");

  if (sfont->type == SFNT_TYPE_TTC) {
    if ((offset = ttc_read_offset(sfont, face)) == 0)
      ERROR("invalid TTC index");
  } else if (sfont->type == SFNT_TYPE_TRUETYPE) {
    if (face > 0) {
      ERROR("found TrueType font file while expecting TTC file");
    } else {
      offset = 0;
    }
  } else {
    ERROR("unexpected error: not TrueType/TTC font");
  }

  if (sfnt_read_table_directory(sfont, offset) < 0)
    ERROR("cannot read TrueType table directory");

  for (i=0;i<LAST_KNOWN_ENCODINGS;i++) {
    if ((ttcmap = tt_cmap_read(sfont,
			       known_encodings[i].platform,
			       known_encodings[i].encoding)) != NULL) {
      break;
    }
  }

  if (ttcmap == NULL || i == LAST_KNOWN_ENCODINGS) { /* cannot continue */
    ERROR("no acceptable TrueType cmap table found");
  }

  if ((cmap_id =
    find_tocode_cmap((cidfont->sysinfo).registry,
                     (cidfont->sysinfo).ordering,
                     known_encodings[i].append)) < 0) {
    ERROR("requested ToCode CMap not found");
  }

  /* build glyphs */

  glyphs = tt_build_init(); /* reset */

  used_chars = NULL; /* h_used_chars & v_used_chars */
  /* horizontal */
  if ((parent_id = (cidfont->parent)[0]) >= 0) {
    if ((h_used_chars = type0_font_used(parent_id)) == NULL) {
      ERROR("unexpected NULL used_char");
    }
    used_chars = h_used_chars;
    for (cid=1;cid<0xffffu;cid++) {
      unsigned char hi = (cid >> 8) & 0xff, lo = cid & 0xff;
      unsigned short gid = 0, code = 0;

      if (is_used_char2(h_used_chars, cid)) {
	if ((code = cmap_lookup(hi, lo, cmap_id)) == CID_NOTDEF ||
	    (gid = tt_cmap_lookup(ttcmap, code)) == 0) {
	  fprintf(stderr, "\n** Warning: some glyph missing **\n");
	}
	tt_add_to_used(glyphs, gid, cid);
      }
    }
  }

  /* vertical */
  if ((parent_id = (cidfont->parent)[1]) >= 0) {
    tt_gsub_t gsub = NULL;

    if ((v_used_chars = type0_font_used(parent_id)) == NULL) {
      ERROR("unexpected NULL used_char");
    }

    /* require `vrt2' or `vert'  */
    if ((gsub = tt_gsub_require_feature(sfont, "vrt2")) == NULL) {
      gsub = tt_gsub_require_feature(sfont, "vert");
    }

    if (gsub) {
      if (verbose > 1)
	fprintf(stderr, "\n  << GSUB feature `vrt2' or `vert' found >>\n");
    } else {
      fprintf(stderr, "\n** Warning: GSUB feature vrt2/vert not found **\n");
    }

    /* 0 ommited, missing glyph, we already have */
    for (cid=1;cid<0xffffu;cid++) {
      unsigned char hi = (cid >> 8) & 0xff, lo = cid & 0xff;
      unsigned short gid = 0, code = 0;

      if (is_used_char2(v_used_chars, cid)) {
	if ((code = cmap_lookup(hi, lo, cmap_id)) == CID_NOTDEF ||
	    (gid = tt_cmap_lookup(ttcmap, code)) == 0) {
	  fprintf(stderr, "\n** Warning: some glyph missing **\n");
	}
	if (gsub)
	  tt_gsub_substitute(gsub, &gid);
	tt_add_to_used(glyphs, gid, cid);
	if (used_chars)
	  add_to_used_chars2(used_chars, cid);
      }
    }
    if (gsub)
      tt_gsub_release(gsub);
    if (used_chars == NULL) /* We have no horizontal */
      used_chars = v_used_chars;
  }

  if (used_chars == NULL) /* totally no used_chars */
    ERROR("unexpected NULL used_chars");

  tt_cmap_release(ttcmap);

  last_cid = glyphs->last_gid; /* backup */

  if (tt_build_tables(sfont, glyphs) < 0) /* fail */
    ERROR("could not created FontFile stream");

#define PDFUNIT(w) ROUND(1000.0*((double)(w))/glyphs->emsize, 1)

  pdf_add_dict (cidfont->fontdict,
		pdf_new_name ("DW"),
		pdf_new_number (PDFUNIT((glyphs->used_glyphs)[0].width)));
  do_w_array(cidfont->w_array, glyphs, (glyphs->used_glyphs)[0].width,
	     last_cid);

  if (verbose > 1)
    fprintf(stderr, "\n  Embeding %u glyphs\n", glyphs->num_glyphs);

  tt_build_finish(glyphs);

  for (i=0;i<LAST_REQ_TABLE;i++) {
    if (sfnt_require_table(sfont,
			   required_table[i].name,
			   required_table[i].must_exist) < 0) {
      ERROR("some required TrueType table does not exist");
    }
  }

  length = sfnt_get_size(sfont);
  result = NEW(length, char);
  if (sfnt_build_font(sfont, result, length) == NULL) { /* fail */
    ERROR("could not created FontFile stream");
  }

  /* success */

  pdf_release_obj(cidfont->fontdict);
  cidfont->fontdict = NULL;

  pdf_release_obj(cidfont->descriptor);
  cidfont->descriptor = NULL;

  /* W */
  if (cidfont->w_array) {
    pdf_release_obj(cidfont->w_array);
    cidfont->w_array = NULL;
  }

  /* CIDSet */
  pdf_add_stream (cidfont->cidset, used_chars, 8192);
  pdf_release_obj(cidfont->cidset);
  cidfont->cidset = NULL;

  /* FontFile2 */
  stream_dict = pdf_stream_dict (cidfont->fontfile);
  pdf_add_dict (stream_dict, pdf_new_name("Length1"),
		pdf_new_number (length));
  pdf_add_stream (cidfont->fontfile, result, length);
  pdf_release_obj(cidfont->fontfile);
  cidfont->fontfile = NULL;
  RELEASE(result); /* add_stream does memcpy */

  /* everything is now ok */

  if (verbose > 1) {
    fprintf(stderr, "  Embedded size: %ld bytes\n", length);
  }

  sfnt_close(sfont);

#ifdef MEM_DEBUG
MEM_END
#endif /* MEM_DEBUG */

  return;
}

int cidtype2_get_id (const char *map_name, int cmap_id)
{
  struct cid_font_s *cidfont;
  char *shortname, *fullname, *fontname, *short_fontname, *attribname;
  unsigned long offset = 0;
  unsigned short namelen;
  sfnt *sfont;
  int face, embedd = 1;

#ifdef MEM_DEBUG
MEM_START
#endif /* MEM_DEBUG */

  shortname = NEW(strlen(map_name)+1, char);
  strcpy(shortname, map_name);

  if ((face = split_face(shortname, &attribname, &embedd)) < 0) {
    if (shortname) RELEASE(shortname);
    return -1;
  }

  if ((fullname = kpse_find_file(shortname, kpse_truetype_format, 1))
      == NULL) {
    RELEASE(shortname);
    return -1;
  }

  if ((sfont = sfnt_open(fullname)) == NULL) {
    fprintf(stderr, "not a TTF file or file may corrupt\n");
    RELEASE(shortname);
    return -1;
  }

  if (sfont->type == SFNT_TYPE_TTC) {
    offset = ttc_read_offset(sfont, face);
  } else if (sfont->type == SFNT_TYPE_TRUETYPE) {
    if (face > 0) {
      fprintf(stderr, "found TrueType font file while expecting TTC file\n");
      RELEASE(shortname);
      sfnt_close(sfont);
      return -1;
    } else {
      offset = 0;
    }
  } else {
    RELEASE(shortname);
    sfnt_close(sfont);
    return -1;
  }

  if (sfnt_read_table_directory(sfont, offset) < 0)
    ERROR("cannot read table directory");

  /* MAC-ROMAN-EN-POSTSCRIPT or WIN-UNICODE-EN(US)-POSTSCRIPT */
  short_fontname = NEW(PDF_NAME_LEN_MAX, char);
  namelen = tt_get_ps_fontname(sfont, short_fontname, PDF_NAME_LEN_MAX);
  if (namelen == 0) {
    fprintf(stderr, "Could not determine font name\n");
    RELEASE(short_fontname);
    RELEASE(shortname);
    sfnt_close(sfont);
    return -1;
  }
  validate_name(&short_fontname, namelen); /* for SJIS, UTF-16, ... string */

  /*
    strlen works, after validate_named string.
    mangled name requires more 7 bytes.
    ',BoldItalic' requires more 11 bytes.
  */
  fontname = NEW(strlen(short_fontname)+12, char);
  strcpy(fontname, short_fontname);
  RELEASE(short_fontname);

  if (attribname) strcat(fontname, attribname);
  
  /*
    ROS are determined from CMap used (it is sufficient for our purpose).
     apparently, Identity-[HV] are not supported !
  */
  cidfont = cid_new_font(CIDFONT_TYPE2, cmap_sysinfo(cmap_id));

  cidfont->ident = NEW (strlen(map_name)+1, char);
  strcpy(cidfont->ident, map_name);

  cidfont->index = face;
  cidfont->filename = shortname;
  cidfont->fontname = fontname;

  cidfont->fontdict = pdf_new_dict();
  cidfont->indirect = pdf_ref_obj(cidfont->fontdict);

  pdf_add_dict (cidfont->fontdict,
		pdf_new_name ("Type"),
		pdf_new_name ("Font"));
  pdf_add_dict (cidfont->fontdict,
		pdf_new_name ("Subtype"),
		pdf_new_name ("CIDFontType2"));

  if ((cidfont->descriptor
       = tt_get_fontdesc(sfont, &embedd, 2)) == NULL) {
    ERROR("cannot get font info");
  }

  if (cidfont->embedd = embedd)
    mangle_fontname(fontname);

  pdf_add_dict (cidfont->descriptor,
		pdf_new_name("FontName"),
		pdf_new_name(fontname));
  pdf_add_dict (cidfont->fontdict, 
		pdf_new_name ("BaseFont"),
		pdf_new_name (fontname));
  pdf_add_dict (cidfont->fontdict, 
		pdf_new_name ("FontDescriptor"),
		pdf_ref_obj(cidfont->descriptor));

  if (cidfont->embedd) {
    cidfont->w_array = pdf_new_array();
    pdf_add_dict (cidfont->fontdict,
		  pdf_new_name ("W"),
		  pdf_link_obj(pdf_ref_obj(cidfont->w_array)));
    cidfont->cidset = pdf_new_stream(STREAM_COMPRESS);
    pdf_add_dict (cidfont->descriptor, pdf_new_name("CIDSet"),
		  pdf_link_obj(pdf_ref_obj(cidfont->cidset)));
    cidfont->fontfile = pdf_new_stream(STREAM_COMPRESS);
    pdf_add_dict (cidfont->descriptor, pdf_new_name("FontFile2"),
		  pdf_link_obj(pdf_ref_obj(cidfont->fontfile)));
  } else {
    cidfont->w_array = NULL;
    cidfont->cidset = NULL;
    cidfont->fontfile = NULL;
  }

  /* write delayed bacause of sysinfo */

  sfnt_close(sfont);

#ifdef MEM_DEBUG
MEM_END
#endif /* MEM_DEBUG */

  return 0; /* success */
}

void cidtype2_release(int cid_id)
{

  return;
}
