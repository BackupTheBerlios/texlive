/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/ttf.c,v 1.1 2004/01/08 13:44:15 rahtz Exp $
    
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
#include <time.h>

#include "system.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "pdfobj.h"
#include "numbers.h"
#include "tfm.h"
#include "pdflimits.h"
#include "twiddle.h"

#include "encodings.h"

/* TrueType */
#include "sfnt.h"
#include "tt_cmap.h"
#include "tt_table.h"
#include "tt_build.h"
#include "tt_aux.h"

#include "ttf.h"

static unsigned char verbose = 0;

void ttf_set_verbose(void)
{
  if (verbose < 255) verbose++;
}

static char partial_enabled = 1;

void ttf_disable_partial (void)
{

  partial_enabled = 0;

  return;
}

static unsigned num_ttfs = 0;
static unsigned max_ttfs = 0;

struct a_ttf
{
  char *fontname;
  pdf_obj *indirect;
  pdf_obj *fontdict;
  pdf_obj *descriptor;
  pdf_obj *fontfile;
  char *ident;
  int embedd;
  int encoding_id;
  char *used_chars;
} *ttfs = NULL;

static void init_a_ttf (struct a_ttf *ttf)
{

  ttf->fontname = NULL;
  ttf->indirect = NULL;
  ttf->fontdict = NULL;
  ttf->descriptor = NULL;
  ttf->fontfile = NULL;
  ttf->ident = NULL;
  ttf->embedd = 1;
  ttf->encoding_id = -1;
  ttf->used_chars = NULL;

  return;
}

static void ttf_release (int id)
{

  if (id >= 0 && id < num_ttfs) {
    if (ttfs[id].ident) RELEASE (ttfs[id].ident);
    if (ttfs[id].fontname) RELEASE (ttfs[id].fontname);
    if (ttfs[id].indirect) pdf_release_obj (ttfs[id].indirect);
    if (ttfs[id].fontdict) pdf_release_obj (ttfs[id].fontdict);
    if (ttfs[id].descriptor) pdf_release_obj (ttfs[id].descriptor);
    if (ttfs[id].fontfile) pdf_release_obj (ttfs[id].fontfile);
    if (ttfs[id].used_chars)
      RELEASE (ttfs[id].used_chars);
  }

  return;
}

static char *new_used_chars (void)
{
  char *result;
  int i;

  result = NEW (256, char);
  for (i=0; i<256; i++) {
    result[i] = 0;
  }

  return result;
}

/*
  PDF_NAME_LEN_MAX: see, Appendix C of PDF Ref. v1.3, 2nd. ed.
  This is Acrobat implementation limit.
*/
#define PDF_NAME_LEN_MAX 127

/*
  PDF viewer applications use following tables

   head, hhea, loca, maxp, glyf, hmtx, fpgm, cvt_, prep, cmap

                                           - from PDF Ref. v.1.3, 2nd ed.

  The fpgm, cvt_, and prep tables appears only when TrueType instructions
  requires them. Those tables must be preserved if they exist.

  The OS/2 table (required for TrueType font for Windows and OS/2) contains
  liscencing information, but PDF viewers seems not using it.
*/

static struct
{
  char *name;
  int must_exist;
} required_table[] = {
  {"OS/2", 1}, {"head", 1}, {"hhea", 1}, {"loca", 1}, {"maxp", 1},
  {"glyf", 1}, {"hmtx", 1}, {"fpgm", 0}, {"cvt ", 0}, {"prep", 0},
  {"cmap", 1}
};
#define LAST_REQ_TABLE (sizeof(required_table)/sizeof(*required_table))

/* also in type1.c */
static void mangle_fontname(char *fontname)
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

  return;
}

/*
  in PDF 1.2 or higher
   All characters not printable in ASCII and some characters that have
   specail meaning in PDF must be converted to #xx form in name string,
   where xx is a hexadecimal notion of a character code.
   The null character (0) is not allowed.

  in PDF 1.1
   Remove all characters which can not be used in name string.

  The new name strings does not contains '\0' in the middle of them.

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

  if (strlen(*name) == 0) {
    ERROR("No valid character found in name string.");
  }

  return;
}

pdf_obj *ttf_font_resource (int ttf_id)
{

  if (ttf_id >= 0 && ttf_id < max_ttfs) {
    return pdf_link_obj(ttfs[ttf_id].indirect);
  } else {
    ERROR ("Invalid font id in ttf_font_resource");
  }

  return NULL;
}

char *ttf_font_used (int ttf_id)
{
  char *result;

  if (ttf_id>=0 && ttf_id<max_ttfs) {
    result = ttfs[ttf_id].used_chars;
  } else {
    fprintf (stderr, "ttf_font_used: ttf_id=%d\n", ttf_id);
    ERROR ("Invalid font id in ttf_font_used");
  }

  return result;
}

int ttf_font (const char *map_name, int tfm_id,
	      char *res_name, int encoding_id, int remap) 
{
  char *fullname, *short_fontname;
  int tfm_firstchar, tfm_lastchar;
  int pdf_firstchar, pdf_lastchar;
  sfnt *sfont;
  pdf_obj *tmp1;
  int namelen;
  int i, embedd = 1;

  if (encoding_id >= 0 && remap)
    return -1;

  for (i=0; i<num_ttfs; i++) {
    if (map_name && ttfs[i].ident && !strcmp (ttfs[i].ident, map_name)
	&& (encoding_id == ttfs[i].encoding_id))
      return i;
  }

  /* This font not previously called for */
  if ((fullname = kpse_find_file (map_name, kpse_truetype_format, 1))
      == NULL || (sfont = sfnt_open(fullname)) == NULL) {
    return -1;
  }

  if (sfont->type != SFNT_TYPE_TRUETYPE ||
      sfnt_read_table_directory(sfont, 0) < 0) {
    return -1;
  }

  short_fontname = NEW(PDF_NAME_LEN_MAX, char);
  namelen = tt_get_ps_fontname (sfont, short_fontname, PDF_NAME_LEN_MAX);
  if (namelen == 0) {
    fprintf(stderr, "\n** Could not find valid PS fontname **\n");
    RELEASE(short_fontname);
    return -1;
  }
  validate_name(&short_fontname, namelen);

  if (num_ttfs >= max_ttfs) {
    max_ttfs += MAX_FONTS;
    ttfs = RENEW (ttfs, max_ttfs, struct a_ttf);
  }

  init_a_ttf (ttfs+num_ttfs);
  ttfs[num_ttfs].ident = NEW (strlen(map_name)+1, char);
  strcpy (ttfs[num_ttfs].ident, map_name);
  ttfs[num_ttfs].encoding_id = encoding_id;

  ttfs[num_ttfs].fontdict = pdf_new_dict();
  ttfs[num_ttfs].indirect = pdf_ref_obj(ttfs[num_ttfs].fontdict);
  if ((ttfs[num_ttfs].descriptor =
       tt_get_fontdesc(sfont, &embedd, 1)) == NULL) {
    ERROR("cannot get font info");
  }
  sfnt_close(sfont);

  if (!(ttfs[num_ttfs].embedd = embedd) && encoding_id >= 0) {
    ERROR("Only built-in encoding allowed for non-embedded TrueType font");
  }

  if (ttfs[num_ttfs].embedd && partial_enabled) {
    ttfs[num_ttfs].fontname = NEW (strlen(short_fontname)+8, char);
    strcpy (ttfs[num_ttfs].fontname, short_fontname);
    mangle_fontname(ttfs[num_ttfs].fontname);
  } else {
    ttfs[num_ttfs].fontname = NEW (strlen(short_fontname)+1, char);
    strcpy (ttfs[num_ttfs].fontname, short_fontname);
  }
  RELEASE (short_fontname);

  pdf_add_dict (ttfs[num_ttfs].fontdict,
		pdf_new_name ("Type"),
		pdf_new_name ("Font"));
  pdf_add_dict (ttfs[num_ttfs].fontdict,
		pdf_new_name ("Subtype"),
		pdf_new_name ("TrueType"));
  pdf_add_dict (ttfs[num_ttfs].fontdict,
		pdf_new_name ("BaseFont"),
		pdf_new_name (ttfs[num_ttfs].fontname));
  pdf_add_dict (ttfs[num_ttfs].fontdict, 
		pdf_new_name ("FontDescriptor"),
		pdf_link_obj(pdf_ref_obj(ttfs[num_ttfs].descriptor)));
  pdf_add_dict (ttfs[num_ttfs].descriptor,
		pdf_new_name("FontName"),
		pdf_new_name(ttfs[num_ttfs].fontname));
  if (ttfs[num_ttfs].embedd) {
    ttfs[num_ttfs].fontfile = pdf_new_stream(STREAM_COMPRESS);
    pdf_add_dict(ttfs[num_ttfs].descriptor, pdf_new_name("FontFile2"),
		 pdf_link_obj(pdf_ref_obj(ttfs[num_ttfs].fontfile)));
  } else {
    ttfs[num_ttfs].fontfile = NULL;
  }

  if (partial_enabled && ttfs[num_ttfs].embedd) {
    ttfs[num_ttfs].used_chars = new_used_chars();
  }
    
  pdf_add_dict (ttfs[num_ttfs].fontdict,
		pdf_new_name ("Name"),
		pdf_new_name (res_name));

  tfm_firstchar = tfm_get_firstchar(tfm_id);
  tfm_lastchar = tfm_get_lastchar(tfm_id);
  if (partial_enabled && remap) {
    unsigned char t;
    pdf_firstchar=255; pdf_lastchar=0;
    for (i=tfm_firstchar; i<=tfm_lastchar; i++) {
      if ((t=twiddle(i)) < pdf_firstchar)
	pdf_firstchar = t;
      if (t > pdf_lastchar)
	pdf_lastchar = t;
    }
  } else {
    pdf_firstchar = tfm_firstchar;
    pdf_lastchar = tfm_lastchar;
  }
  pdf_add_dict (ttfs[num_ttfs].fontdict,
		pdf_new_name ("FirstChar"),
		pdf_new_number (pdf_firstchar));
  pdf_add_dict (ttfs[num_ttfs].fontdict,
		pdf_new_name ("LastChar"),
		pdf_new_number (pdf_lastchar));
  tmp1 = pdf_new_array ();
  for (i=pdf_firstchar; i<=pdf_lastchar; i++) {
    if (partial_enabled && remap) {
      int t;
      if ((t=untwiddle(i)) <= tfm_lastchar && t>=tfm_firstchar) {
	pdf_add_array (tmp1,
		       pdf_new_number(ROUND(tfm_get_width
					    (tfm_id, t)*1000.0,1.)));
      } else {
	pdf_add_array (tmp1, pdf_new_number(0.0));
      }
    } else {
      pdf_add_array (tmp1,
		     pdf_new_number(ROUND(tfm_get_width
					  (tfm_id, i)*1000.0,1.)));
    }
  }
  pdf_add_dict (ttfs[num_ttfs].fontdict, pdf_new_name ("Widths"), tmp1);
  num_ttfs += 1;

  return (num_ttfs - 1);
}

/* #include "winansi.h" */

void do_ttf (int ttf_id)
{
  char *result;
  char *fullname;
  pdf_obj *stream_dict;
  sfnt *sfont;
  ULONG length = 0;
  int i;

#ifdef MEM_DEBUG
MEM_START
#endif /* MEM_DEBUG */
  
  if (!ttfs[ttf_id].embedd) {/* no embedd */
    return;
  }

  fullname = kpse_find_file (ttfs[ttf_id].ident,
			     kpse_truetype_format, 1);
  if (verbose == 1)
    fprintf (stderr, "(TTF:%s", ttfs[ttf_id].ident);
  if (verbose > 1)
    fprintf (stderr, "(TTF:%s", fullname);

  if (fullname == NULL ||
      (sfont = sfnt_open(fullname)) == NULL) {
    fprintf (stderr, "Unable to find or open binary font file (%s)",
	     ttfs[ttf_id].ident);
    ERROR ("This existed when I checked it earlier!");
    return;
  }

  if (sfont->type != SFNT_TYPE_TRUETYPE || 
      sfnt_read_table_directory(sfont, 0) < 0)
    ERROR("Unexpected error: not a TrueType font ?");

  /* build glyphs */
  if (partial_enabled && ttfs[ttf_id].used_chars) {
    struct tt_glyphs *glyphs;
    char *cmap_table;
    int count;

    /* New cmap table with MacRoman encoding */
    cmap_table = NEW(274, char);
    memset(cmap_table, 0, 274);
    sfnt_put_ushort(cmap_table, 0); /* version */
    sfnt_put_ushort(cmap_table+2, 1); /* number of subtables */
    sfnt_put_ushort(cmap_table+4, TT_MAC); /* platform ID */
    sfnt_put_ushort(cmap_table+6, TT_MAC_ROMAN); /* encoding ID */
    sfnt_put_ulong(cmap_table+8, 12); /* offset */
    sfnt_put_ushort(cmap_table+12, 0); /* format */
    sfnt_put_ushort(cmap_table+14, 262); /* length */
    sfnt_put_ushort(cmap_table+16, 0); /* language */

    glyphs = tt_build_init();
    count = 1;
    if (verbose > 2)
      fprintf(stderr, "\n/.notdef");
    if (ttfs[ttf_id].encoding_id < 0) { /* by character code */
      tt_cmap *tt_cmap;
      if ((tt_cmap = tt_cmap_read(sfont, TT_MAC, TT_MAC_ROMAN))
	  == NULL)
	ERROR("Cannot read TrueType cmap table");
      for (i=0;i<256;i++) {
	unsigned short gid;
	if (ttfs[ttf_id].used_chars[i]) {
	  if (verbose > 2)
	    fprintf(stderr, "/.c0x%02x", i);
	  gid = tt_cmap_lookup(tt_cmap, (unsigned short)i);
	  if (gid == 0) {
	    fprintf(stderr, "\n** Warning: character 0x%02x missing **\n", i);
	  }
	  tt_add_to_used(glyphs, gid, (unsigned short)count);
	  cmap_table[18+i] = count;
	  count++;
	}
      }
      tt_cmap_release(tt_cmap);
    } else { /* by glyph names form post table */
      struct tt_glyph_names *glyph_names;
      if ((glyph_names = tt_get_glyph_names(sfont)) == NULL)
	ERROR("PostScript glyph name list not available");
      for (i=0;i<256;i++) {
	char *name;
	unsigned short gid;
	if (ttfs[ttf_id].used_chars[i]) {
	  name = encoding_glyph(ttfs[ttf_id].encoding_id, i);
	  if (verbose > 2)
	    fprintf(stderr, "/%s", name);
	  gid = tt_glyph_lookup(glyph_names, name);
	  if (gid == 0) {
	    fprintf(stderr, "\n** Warning: glyph `%s' missing **\n", name);
	  }
	  tt_add_to_used(glyphs, gid, (unsigned short)count);
	  cmap_table[18+i] = count;
	  count++;
	}
      }
      tt_release_glyph_names(glyph_names);
    }
    if (tt_build_tables(sfont, glyphs) < 0) /* fail */
      ERROR("could not created FontFile stream");
    if (verbose > 1) 
      fprintf(stderr, "\n  Embedding %d glyphs\n", glyphs->num_glyphs);
    tt_build_finish(glyphs);
    sfnt_set_table(sfont, "cmap", cmap_table, 274);
  }

  for (i=0;i<LAST_REQ_TABLE;i++) {
    if (sfnt_require_table(sfont,
			   required_table[i].name,
			   required_table[i].must_exist) < 0) {
      ERROR("some required TrueType table does not exist");
    }
  }
  if (!partial_enabled) {
    sfnt_require_table(sfont, "post", 0);
    sfnt_require_table(sfont, "name", 0);
  }

  length = sfnt_get_size(sfont);
  result = NEW(length, char);
  if (sfnt_build_font(sfont, result, length) == NULL) { /* fail */
    ERROR("could not created FontFile stream");
  }
  sfnt_close(sfont);

  pdf_release_obj(ttfs[ttf_id].fontdict);
  ttfs[ttf_id].fontdict = NULL;

  pdf_release_obj(ttfs[ttf_id].descriptor);
  ttfs[ttf_id].descriptor = NULL;

  /* FontFile2 */
  stream_dict = pdf_stream_dict (ttfs[ttf_id].fontfile);
  pdf_add_dict (stream_dict, pdf_new_name("Length1"),
		pdf_new_number (length));
  pdf_add_stream (ttfs[ttf_id].fontfile, result, length);
  pdf_release_obj(ttfs[ttf_id].fontfile);
  ttfs[ttf_id].fontfile = NULL;
  RELEASE(result); /* add_stream does memcpy */

  if (verbose > 1) {
    fprintf(stderr, "  Embedded size: %ld bytes\n", length);
  }
  if (verbose)
    fprintf (stderr, ")");

#ifdef MEM_DEBUG
MEM_END
#endif /* MEM_DEBUG */

  return;
}

void ttf_close_all (void)
{
  int i;

  for (i=0; i<num_ttfs; i++) {
    do_ttf(i);
    ttf_release (i);
  }
  if (ttfs)
    RELEASE (ttfs);

  return;
}
