/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/type0.c,v 1.1 2004/01/08 13:44:15 rahtz Exp $
    
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
  Type0 (Composite) font support:

  TODO:
    composite font (multiple descendants) - not supported in PDF
*/

#include <string.h>
#include <stdio.h>

#include "system.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"

#ifndef PDF_LOWEST_MINOR_VERSION
#define PDF_LOWEST_MINOT_VERSION 2
#endif

/* only for MAX macro ! */
#include "numbers.h"
#include "pdfobj.h"

/* only for MAX_FONTS */
#include "pdflimits.h"

#ifdef PTEX_ONLY
#include "tfm.h"
#endif /* PTEX_ONLY */

#include "cmap.h"
#include "cid.h"

#include "type0.h"

static unsigned char verbose = 0;

void type0_set_verbose(void)
{
  cid_set_verbose(); /* propagate */
  if (verbose < 255) {
    verbose += 1;
  }
}

#define CMAP_ADOBE_IDENTITY_UCS2 "Adobe-Identity-UCS2"
static pdf_obj *tounicode_cmap_identity = NULL;

static void load_tounicode_cmap_identity(void)
{
  char *buffer, *full_filename;
  FILE *fp;
  unsigned long length;
  pdf_obj *stream;

#ifdef MIKTEX
  full_filename = work_buffer;
  if (!miktex_find_app_input_file("dvipdfm", CMAP_ADOBE_IDENTITY_UCS2, full_filename)) {
    if (miktex_get_acrobat_font_dir(full_filename)) {
      strcat(full_filename, "\\..\\CMap\\");
      strcat(full_filename, CMAP_ADOBE_IDENTITY_UCS2);
    } else
      full_filename = NULL;
  }
#else
  full_filename = kpse_find_file(CMAP_ADOBE_IDENTITY_UCS2, kpse_program_text_format, 1);
#endif

  /* don't stop here ! someone just test existence of CMap */
  if (full_filename == NULL ||
      (fp = MFOPEN (full_filename, FOPEN_RBIN_MODE)) == NULL ||
      (length = file_size(fp)) == 0) {
    fprintf(stderr, "\n**Fatal: Could not find the ToUnicode CMap file: %s.",
            CMAP_ADOBE_IDENTITY_UCS2);
    ERROR ("\n");
  }

  buffer = NEW (length, char); 
  fread (buffer, sizeof (char), length, fp);
  MFCLOSE (fp);

  stream = pdf_new_stream(STREAM_COMPRESS);
  tounicode_cmap_identity = pdf_link_obj(pdf_ref_obj(stream));

  pdf_add_dict(pdf_stream_dict(stream),
	       pdf_new_name("Length"), pdf_new_number(length));
  pdf_add_stream(stream, buffer, length);
  pdf_release_obj(stream);
  RELEASE (buffer);
}

/*
  All CJK double-byte characters are mapped so that resulting character
  codes are coinside with CIDs of given character collection.
  Hence /Encoding is always /Identity-H for horizontal fonts and for vertical
  fonts, /Identity-V.
*/

static char *enc_identity_h = "Identity-H";
static char *enc_identity_v = "Identity-V";

/*
  used_chars:
   We use bitmap rather than used_chars array. Since CID-keyed fonts contains
   many characters and it can be reused as content of CIDSet stream.
   See, cid.h for add_to_used and is_used which is macro.
*/

char *new_used_chars2(void)
{
  char *data;

  data = NEW(8192, char);
  memset(data, 0, 8192);

  return data;
}

int num_type0_fonts = 0, max_type0_fonts;

/* Composite fonts are not supported */
struct a_type0_font 
{
  pdf_obj *indirect; /* Indirect object */
  char *encoding;    /* Identity-H or Identity-V */
  char *used_chars;  /* Used chars (CIDs) */
  int descendant;    /* Only single descendant is allowed */
  int wmode;         /* Writing mode, horizontal: 0, vertical: 1 */
  int flag;          /* 0: used_chars is only for myself */
} *type0_fonts = NULL;

static void init_a_type0_font (struct a_type0_font *this_type0_font)
{
  this_type0_font->indirect = NULL;
  this_type0_font->encoding = NULL;
  this_type0_font->used_chars = NULL;
  this_type0_font->descendant = -1;
  this_type0_font->wmode = 0;
  this_type0_font->flag = 0;
}

pdf_obj *type0_font_resource (int type0_id)
{
  if (type0_id >= 0 && type0_id < num_type0_fonts) {
    return pdf_link_obj(type0_fonts[type0_id].indirect);
  } else {
    fprintf (stderr, "type0_font_resource(): type0_id=%d", type0_id);
    ERROR ("Invalid font id in type1_font_resource()");
  }

  return NULL;
}

int type0_font_wmode (int type0_id)
{
  int result = 0;

  if (type0_id >= 0 && type0_id < num_type0_fonts) {
    result = type0_fonts[type0_id].wmode;
  } else {
    fprintf (stderr, "type0_font_wmode(): type0_id=%d", type0_id);
    ERROR ("Invalid font id in type0_font_encoding()");
  }

  return result;
}

char *type0_font_encoding (int type0_id)
{
  char *result = NULL;

  if (type0_id >= 0 && type0_id < num_type0_fonts) {
    result = type0_fonts[type0_id].encoding;
  } else {
    fprintf (stderr, "type0_font_encoding(): type0_id=%d", type0_id);
    ERROR ("Invalid font id in type0_font_encoding()");
  }

  return result;
}

char *type0_font_used (int type0_id)
{
  char *result = NULL;

  if (type0_id >= 0 && type0_id < num_type0_fonts) {
    result = type0_fonts[type0_id].used_chars;
  } else {
    fprintf (stderr, "type0_font_used(): type0_id=%d", type0_id);
    ERROR ("Invalid font id in type0_font_used()");
  }

  return result;
}

/* Adobe Type0 Font Resources */
int type0_font (const char *map_name, int tfm_font_id, char
		*resource_name, int cmap_id, int remap)
{
  int result = -1;
  char *fontname;
  char *encoding;
  pdf_obj *font_resource, *descendant_array;
  int cid_id = -1, parent_id = -1, wmode = 0;

  /*
    Encoding is Identity-H or Identity-V according as thier WMode value.

    Font which covers characters across multiple character collection
    (eg, Adobe-Japan1 and Adobe-Japan2) must be splited into multiple
    CID-keyed fonts.

    cidxxx_get_id() called within cid_get_id() should check the
    compatibility of character collections.
  */

  /* We also pass the cmap_id */
  if (map_name == NULL || cmap_id < 0 || /* no default */
      (cid_id = cid_get_id(map_name, cmap_id)) < 0) {
    return -1;
  }

  /*
    CID-keyed font has been attached or already been registerd.
    If CID-keyed font with ID = cid_id is new font, then create new parent
    Type 0 font. Otherwise, there already exists parent Type 0 font and
    then find him and return his ID. We must check against their encodings.
  */

  /* get WMode from TFM (ASCII pTeX specific) */
#ifdef PTEX_ONLY
  if ((wmode = is_vertical(tfm_font_id)) == 1) {
    encoding = enc_identity_v;
  } else {
    wmode = 0;
    encoding = enc_identity_h;
  }
#else
  /* or get WMode from CMap's WMode           */
  if ((wmode = cmap_wmode(cmap_id)) == 0) {
    encoding = enc_identity_h;
  } else if (wmode == 1) {
    encoding = enc_identity_v;
  } else {
    ERROR("invalid WMode");
  }
#endif /* PTEX_ONLY */

  /* Does CID-keyed font already have parent ? */
  parent_id = cid_parent_id(cid_id, wmode); /* compare also encoding */
  if (parent_id >= 0) { /* if so, */
    return parent_id;   /* we don't need new one */
  }

  /*
     We might hvae Type 0 font that have same descendant CID-keyed font
     but his parent's encoding does not match our new Type 0 font.
     Create new one.
  */

  if (num_type0_fonts >= max_type0_fonts) {
    max_type0_fonts = MAX (max_type0_fonts+MAX_FONTS, num_type0_fonts+1);
    type0_fonts = RENEW (type0_fonts, max_type0_fonts, struct a_type0_font);
  }

  init_a_type0_font (type0_fonts+num_type0_fonts);
  type0_fonts[num_type0_fonts].descendant = cid_id;

  /* start Font Dict */
  font_resource = pdf_new_dict ();
  pdf_add_dict (font_resource,
		pdf_new_name ("Type"),
		pdf_new_name ("Font"));
  pdf_add_dict (font_resource,
		pdf_new_name ("Subtype"),
		pdf_new_name ("Type0"));

  /*
    Name:
     Name used in font resource dictionary - required only in PDF-1.0
     Note: This entry is obsolescent and its use is no longer recommended.
     (See implementation note 39 in Appendix H., PDF Reference v1.3, 2nd ed.)
  */
#if (PDF_LOWEST_MINOR_VERSION < 1)
  pdf_add_dict (font_resource,
		pdf_new_name ("Name"),
		pdf_new_name (resource_name));
#endif /* PDF_LOWEST_MINOR_VERSION < 1 */

  /*
    Type0 font does not have FontDescriptor because it is not a font.
    Instead, DescendantFonts appears here.
    
    Up to PDF-1.4, Type0 font must have single descendant font which is
    CID-keyed font. Future PDF spec. will allow multiple desecendant fonts.
  */
  descendant_array = pdf_new_array ();
  pdf_add_array(descendant_array,
		cid_descendant(num_type0_fonts, cid_id, wmode));
  pdf_add_dict (font_resource, pdf_new_name ("DescendantFonts"),
		descendant_array);
  
  /*
    PostScript Font name:
     Type0 font fontname is usually descendant CID-keyed font's fontname
     plus `-encoding'.
  */
  fontname = cid_fontname(cid_id);

  if (verbose) {
    if (cid_embedd(cid_id) && strlen(fontname) > 7) {
      fprintf(stderr, "(CID:%s)", fontname+7); /* skip XXXXXX+ */
    } else {
      fprintf(stderr, "(CID:%s)", fontname);
    }
  }

  /*
    Type 0 font is actually CID-keyed font since descendant font is
    restricted to a single CID-keyed font. As we currently convert all
    character codes to CIDs for Type 0 font, we keep track only one parent
    Type 0 font for CIDFontType 0 font and use same used_chars bitmap for
    all parents. CIDFontType 2 must be treated differently, because it is
    TrueType which requires some extra process to access vertical glyphs.

    In most PDF file, encoding name is not appended to fontname for Type0
    fonts having CIDFontType 2 font as their descendant.
  */

  type0_fonts[num_type0_fonts].used_chars = NULL;

  switch (cid_fonttype(cid_id)) {
  case CIDFONT_BASE: /* continue */
  case CIDFONT_TYPE0:
    {
      char *fullname;
      int len;

      len = strlen(fontname)+strlen(encoding)+1; /* don't forget `-' */
      fullname = NEW (len+1, char);
      sprintf(fullname, "%s-%s", fontname, encoding);
      pdf_add_dict (font_resource, 
		    pdf_new_name ("BaseFont"),
		    pdf_new_name (fullname));
      RELEASE(fullname);
      
      if (cid_embedd(cid_id)) {
	if ((parent_id = cid_parent_id(cid_id, ((wmode) ? 0: 1))) < 0) {
	  type0_fonts[num_type0_fonts].used_chars = new_used_chars2();
	} else {
	  /* don't allocate new one */
	  type0_fonts[num_type0_fonts].used_chars =
	    type0_fonts[parent_id].used_chars;
	  type0_fonts[num_type0_fonts].flag = 1;
	}
      }
    }
    break;
  /*
    TrueType:
     o Different used_chars bitmap for h, v.
  */
  case CIDFONT_TYPE2:
    pdf_add_dict (font_resource,
		  pdf_new_name ("BaseFont"), pdf_new_name (fontname));
    if (cid_embedd(cid_id))
      type0_fonts[num_type0_fonts].used_chars = new_used_chars2();
	break;
  default:
    ERROR("unrecognized CIDFont Type");
    break;
  }

  /* ToUnicode */
  if (cmap_id == CMAP_IDENTITY_H || cmap_id == CMAP_IDENTITY_V) {
    if (tounicode_cmap_identity == NULL)
      load_tounicode_cmap_identity();
    pdf_add_dict (font_resource, pdf_new_name("ToUnicode"),
		  tounicode_cmap_identity);
  }

  /* finaly write Encoding */
  pdf_add_dict (font_resource,
		pdf_new_name ("Encoding"),
		pdf_new_name(encoding));
  /* Font Dict finished */
  
  type0_fonts[num_type0_fonts].indirect = pdf_ref_obj(font_resource);
  pdf_release_obj (font_resource);

  result = num_type0_fonts;
  type0_fonts[num_type0_fonts].encoding = encoding;
  type0_fonts[num_type0_fonts].wmode = wmode;

  num_type0_fonts++;

  return result;
}

void type0_close_all (void)
{
  int i;

  /* cid_flush_all comes first, because of used_chars */
  cid_flush_all();
  for (i=0; i<num_type0_fonts; i++) {
    if (type0_fonts[i].indirect)
      pdf_release_obj(type0_fonts[i].indirect);

    /* avoid free-ing aleady free-ed used_chars */
    if (type0_fonts[i].flag == 0 && type0_fonts[i].used_chars)
      RELEASE(type0_fonts[i].used_chars);
  }

  if (type0_fonts)
    RELEASE (type0_fonts);
}
