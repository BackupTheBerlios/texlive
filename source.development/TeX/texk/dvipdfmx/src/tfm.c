/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/tfm.c,v 1.1 2004/01/08 13:44:15 rahtz Exp $
    
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

#include <stdio.h>

#include "system.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "pdflimits.h"
#include "numbers.h"
#include "tfm.h"

#define FWBASE ((double) (1<<20))
#define JFM_ID 11
#define JFMV_ID 9 /* vertical version of JFM */

static unsigned char verbose = 0;

/* TFM Record structure
     Multiple TFM's may be read in at once */

struct a_tfm
{
  UNSIGNED_BYTE id; /* id = 11(9) for jfm(vertical) */
  UNSIGNED_BYTE nt; /* number of charcter types for jfm */
  UNSIGNED_QUAD wlenfile, wlenheader;
  UNSIGNED_QUAD bc, ec;
  UNSIGNED_QUAD nwidths, nheights, ndepths;
  UNSIGNED_QUAD nitcor, nlig, nkern, nextens;
  UNSIGNED_QUAD nfonparm;
  UNSIGNED_QUAD font_direction;	/* Used only in OFMs.  TFMs don't have
				 this field*/
  UNSIGNED_QUAD nco, ncw, npc;
  SIGNED_QUAD *header;
  UNSIGNED_QUAD *char_info;
  UNSIGNED_PAIR *width_index;
  UNSIGNED_BYTE *height_index;
  UNSIGNED_BYTE *depth_index;
  SIGNED_QUAD *width;
  SIGNED_QUAD *height;
  SIGNED_QUAD *depth;
  char *tex_name;
  UNSIGNED_PAIR *chartypes;
  fixword *unpacked_widths;
  fixword *unpacked_heights;
  fixword *unpacked_depths;
};

void a_tfm_init (struct a_tfm *a_tfm) 
{
  a_tfm->id = 0;
  a_tfm->nt = 0;
  a_tfm->header = NULL;
  a_tfm->char_info = NULL;
  a_tfm->width_index = NULL;
  a_tfm->height_index = NULL;
  a_tfm->depth_index = NULL;
  a_tfm->width = NULL;
  a_tfm->height = NULL;
  a_tfm->depth = NULL;
  a_tfm->chartypes = NULL;
  a_tfm->unpacked_widths = NULL;
  a_tfm->unpacked_heights = NULL;
  a_tfm->unpacked_depths = NULL;
}


struct a_tfm *tfm = NULL;
static unsigned numtfms = 0, max_tfms = 0; /* numtfms should equal
					      numfonts in dvi.c */
static void tfms_need (unsigned n)
{
  if (n > max_tfms) {
    max_tfms = MAX (max_tfms+MAX_FONTS, n);
    tfm = RENEW (tfm, max_tfms, struct a_tfm);
  }
}

/* External Routine */

void tfm_set_verbose(void)
{
  if (verbose < 255) verbose++;
}

static UNSIGNED_PAIR sum_of_tfm_sizes (struct a_tfm *a_tfm)
{
  unsigned long result = 6;
  result += (a_tfm -> ec - a_tfm -> bc + 1);
  result += a_tfm -> wlenheader;
  result += a_tfm -> nwidths;
  result += a_tfm -> nheights;
  result += a_tfm -> ndepths;
  result += a_tfm -> nitcor;
  result += a_tfm -> nlig;
  result += a_tfm -> nkern;
  result += a_tfm -> nextens;
  result += a_tfm -> nfonparm;
  if (a_tfm->id == JFM_ID || a_tfm->id == JFMV_ID) {
    result += a_tfm -> nt + 1;
  }
  return result;
}

static SIGNED_QUAD sum_of_ofm_sizes (struct a_tfm *a_tfm)
{
  unsigned long result = 14;
  result += 2*(a_tfm -> ec - a_tfm -> bc + 1);
  result += a_tfm -> wlenheader;
  result += a_tfm -> nwidths;
  result += a_tfm -> nheights;
  result += a_tfm -> ndepths;
  result += a_tfm -> nitcor;
  result += 2*(a_tfm -> nlig);
  result += a_tfm -> nkern;
  result += 2*(a_tfm -> nextens);
  result += a_tfm -> nfonparm;
  return result;
}


static void get_sizes (FILE *tfm_file, SIGNED_QUAD tfm_file_size,
		       struct a_tfm *a_tfm)
{
  UNSIGNED_PAIR tfm_id;

  tfm_id = get_unsigned_pair (tfm_file);
  if (tfm_id == JFM_ID || tfm_id == JFMV_ID) { /* is jfm */
    a_tfm -> id = tfm_id;
    a_tfm -> nt = get_unsigned_pair (tfm_file);
    a_tfm -> wlenfile = get_unsigned_pair (tfm_file);
#ifdef DEBUG
    fprintf (stderr, "size: %ld\n", a_tfm->wlenfile);
#endif
  } else {
    a_tfm -> wlenfile =  tfm_id;
  }
  a_tfm -> wlenheader = get_unsigned_pair (tfm_file);
  a_tfm -> bc = get_unsigned_pair (tfm_file);
  a_tfm -> ec = get_unsigned_pair (tfm_file);
  if (a_tfm -> ec < a_tfm -> bc) {
    ERROR ("TFM file error (ec < bc)\n");
  }
  a_tfm -> nwidths = get_unsigned_pair (tfm_file);
  a_tfm -> nheights = get_unsigned_pair (tfm_file);
  a_tfm -> ndepths = get_unsigned_pair (tfm_file);
  a_tfm -> nitcor = get_unsigned_pair (tfm_file);
  a_tfm -> nlig = get_unsigned_pair (tfm_file);
  a_tfm -> nkern = get_unsigned_pair (tfm_file);
  a_tfm -> nextens = get_unsigned_pair (tfm_file);
  a_tfm -> nfonparm = get_unsigned_pair (tfm_file);
#ifdef DEBUG
  fprintf (stderr, "\nComputed size (words)%d\n", sum_of_tfm_sizes (a_tfm));
  fprintf (stderr, "Stated size (words)%ld\n", a_tfm -> wlenfile);
  fprintf (stderr, "Actual size (bytes)%ld\n", tfm_file_size);
#endif
  if ( a_tfm -> wlenfile != tfm_file_size/4 ||
       sum_of_tfm_sizes (a_tfm) != a_tfm -> wlenfile) {
    if (tfm_file_size/4 > a_tfm -> wlenfile) {
      fprintf (stderr, "\nHmm.  A TFM file is larger than it says it is!");
      fprintf (stderr, "\nProceeding nervously...\n");
    } else {
      ERROR ("TFM file problem.  Table sizes don't agree.\n");
    }
  }
  return;
}

static int ofm_get_sizes (FILE *ofm_file,  UNSIGNED_QUAD ofm_file_size,
			   struct a_tfm *a_tfm)
{
  SIGNED_QUAD level;
  level = get_signed_quad (ofm_file);
  a_tfm -> wlenfile = get_signed_quad (ofm_file);
  a_tfm -> wlenheader = get_signed_quad (ofm_file);
  a_tfm -> bc = get_signed_quad (ofm_file);
  a_tfm -> ec = get_signed_quad (ofm_file);
  if (a_tfm -> ec < a_tfm -> bc) {
    ERROR ("OFM file error (ec < bc)\n");
  }
  a_tfm -> nwidths = get_signed_quad (ofm_file);
  a_tfm -> nheights = get_signed_quad (ofm_file);
  a_tfm -> ndepths = get_signed_quad (ofm_file);
  a_tfm -> nitcor = get_signed_quad (ofm_file);
  a_tfm -> nlig = get_signed_quad (ofm_file);
  a_tfm -> nkern = get_signed_quad (ofm_file);
  a_tfm -> nextens = get_signed_quad (ofm_file);
  a_tfm -> nfonparm = get_signed_quad (ofm_file);
  a_tfm -> font_direction = get_signed_quad (ofm_file);
  if (a_tfm->font_direction) {
    fprintf (stderr, "Warning:  I may be interpreting a font direction incorrectly.\n");
  }
  if (level == 0) {
    if (a_tfm -> wlenfile != ofm_file_size/4 ||
	sum_of_ofm_sizes (a_tfm) != a_tfm -> wlenfile) {
      ERROR ("OFM file problem.  Table sizes don't agree.\n");
    }
#ifdef DEBUG
    fprintf (stderr, "Computed size (words)%ld\n", sum_of_ofm_sizes (a_tfm));
    fprintf (stderr, "Stated size (words)%ld\n", a_tfm -> wlenfile);
    fprintf (stderr, "Actual size (bytes)%ld\n", ofm_file_size);
#endif
  } else if (level == 1) {
    a_tfm -> nco = get_signed_quad (ofm_file);
    a_tfm -> ncw = get_signed_quad (ofm_file);
    a_tfm -> npc = get_signed_quad (ofm_file);
    seek_absolute (ofm_file, 4*(a_tfm -> nco));
  } else {
    ERROR ("Can't handle OFM files with level > 1");
  }
  return (int) (level);
}

static void dump_sizes (struct a_tfm *a_tfm)
{
  fprintf (stderr, "\nwlenfile: %ld, ", a_tfm -> wlenfile);
  fprintf (stderr, "wlenheader: %ld\n", a_tfm -> wlenheader);
  fprintf (stderr, "bc: %ld, ", a_tfm -> bc);
  fprintf (stderr, "ec: %ld, ", a_tfm -> ec);
  fprintf (stderr, "nwidths: %ld, ", a_tfm -> nwidths);
  fprintf (stderr, "nheights: %ld, ", a_tfm -> nheights);
  fprintf (stderr, "ndepths: %ld\n", a_tfm -> ndepths);
  fprintf (stderr, "nitcor: %ld, ", a_tfm -> nitcor);
  fprintf (stderr, "nlig: %ld, ", a_tfm -> nlig);
  fprintf (stderr, "nkern: %ld, ", a_tfm -> nkern);
  fprintf (stderr, "nextens: %ld, ", a_tfm -> nextens);
  fprintf (stderr, "nfonparm: %ld\n", a_tfm -> nfonparm);
  return;
}


static void get_fix_word_array (FILE *tfm_file, SIGNED_QUAD *a_word,
				SIGNED_QUAD length)
{
  unsigned i;
  for (i=0; i< length; i++) {
    a_word[i] = get_signed_quad (tfm_file);
  }
  return;
}

static void get_unsigned_quad_array (FILE *tfm_file, UNSIGNED_QUAD *a_word,
				     SIGNED_QUAD length)
{
  unsigned i;
  for (i=0; i< length; i++) {
    a_word[i] = get_unsigned_quad (tfm_file);
  }
  return;
}

static void do_fix_word_array (FILE *tfm_file, SIGNED_QUAD **a, SIGNED_QUAD len)
{
  if (len != 0) {
    *a = NEW (len, SIGNED_QUAD);
    get_fix_word_array (tfm_file, *a, len);
  } else
    *a = NULL;
  return;
}

static void do_unsigned_quad_array (FILE *tfm_file, UNSIGNED_QUAD **a, UNSIGNED_PAIR len)
{
  if (len != 0) {
    *a = NEW (len, UNSIGNED_QUAD);
    get_unsigned_quad_array (tfm_file, *a, len);
  } else
    *a = NULL;
  return;
}

static void do_char_type_array(FILE *tfm_file, struct a_tfm *a_tfm)
{
  int i;
  UNSIGNED_PAIR charcode;
  UNSIGNED_PAIR chartype;
  a_tfm -> chartypes = NEW (65536, UNSIGNED_PAIR);
  for (i=0; i<65536; i++) {
    (a_tfm->chartypes)[i] = 0;
  }
  for (i=0; i < a_tfm->nt; i++ ) {
    charcode = get_unsigned_pair(tfm_file);
    chartype = get_unsigned_pair(tfm_file);
    (a_tfm->chartypes)[charcode] = chartype;
  }
}

static void unpack_widths(struct a_tfm *a_tfm)
{
  int i;
  UNSIGNED_QUAD charinfo;
  UNSIGNED_PAIR width_index;
  a_tfm -> unpacked_widths = NEW (256, fixword);
  for (i=0; i<256; i++) {
    (a_tfm ->unpacked_widths)[i] = 0;
  }
  for (i=(a_tfm->bc); i<=(a_tfm->ec); i++ ) {
    charinfo = (a_tfm->char_info)[i-(a_tfm->bc)];
    width_index = (charinfo / 16777216ul);
    (a_tfm->unpacked_widths)[i] = (a_tfm->width)[width_index];
  }
  return;
}

static void unpack_heights(struct a_tfm *a_tfm)
{
  int i;
  UNSIGNED_QUAD charinfo;
  UNSIGNED_PAIR height_index;
  a_tfm -> unpacked_heights = NEW (256, fixword);
  for (i=0; i<256; i++) {
    (a_tfm ->unpacked_heights)[i] = 0;
  }
  for (i=(a_tfm->bc); i<=(a_tfm->ec); i++ ) {
    charinfo = (a_tfm->char_info)[i-(a_tfm->bc)];
    height_index = (charinfo / 0x100000ul) & 0xf;
    (a_tfm->unpacked_heights)[i] = (a_tfm->height)[height_index];
  }
  return;
}

static void unpack_depths(struct a_tfm *a_tfm)
{
  int i;
  UNSIGNED_QUAD charinfo;
  UNSIGNED_PAIR depth_index;
  a_tfm -> unpacked_depths = NEW (256, fixword);
  for (i=0; i<256; i++) {
    (a_tfm ->unpacked_depths)[i] = 0;
  }
  for (i=(a_tfm->bc); i<=(a_tfm->ec); i++ ) {
    charinfo = (a_tfm->char_info)[i-(a_tfm->bc)];
    depth_index = (charinfo / 0x10000ul) & 0xf;
    (a_tfm->unpacked_depths)[i] = (a_tfm->depth)[depth_index];
  }
  return;
}

static void get_arrays (FILE *tfm_file, struct a_tfm *a_tfm)
{
#ifdef DEBUG
  fprintf (stderr, "Reading %ld word header\n", a_tfm->wlenheader);
#endif
  do_fix_word_array (tfm_file, &(a_tfm -> header), a_tfm -> wlenheader);
  if (a_tfm->id == JFM_ID || a_tfm->id == JFMV_ID) {
    do_char_type_array (tfm_file, a_tfm);
  }
#ifdef DEBUG
  fprintf (stderr, "Reading %ld char_infos\n", (a_tfm->ec)-(a_tfm->bc)+1);
#endif
  do_unsigned_quad_array (tfm_file, &(a_tfm -> char_info), (a_tfm->ec)-(a_tfm->bc)+1);
#ifdef DEBUG
  fprintf (stderr, "Reading %ld widths\n", a_tfm -> nwidths);
#endif
  do_fix_word_array (tfm_file, &(a_tfm -> width), a_tfm -> nwidths);
#ifdef DEBUG
  fprintf (stderr, "Reading %ld heights\n", a_tfm -> nheights);
#endif
  do_fix_word_array (tfm_file, &(a_tfm -> height), a_tfm -> nheights);
#ifdef DEBUG
  fprintf (stderr, "Reading %ld depths\n", a_tfm -> ndepths);
#endif
  do_fix_word_array (tfm_file, &(a_tfm -> depth), a_tfm -> ndepths);
  unpack_widths (a_tfm);
  unpack_heights (a_tfm);
  unpack_depths (a_tfm);
  return;
}

static void do_ofm_zero_char_info (FILE *tfm_file, struct a_tfm *a_tfm)
{
  unsigned i;
  UNSIGNED_QUAD num_chars;
  num_chars = a_tfm->ec - a_tfm->bc + 1;
  if (num_chars != 0) {
    a_tfm -> width_index = NEW (num_chars, UNSIGNED_PAIR);
    a_tfm -> height_index = NEW (num_chars, UNSIGNED_BYTE);
    a_tfm -> depth_index = NEW (num_chars, UNSIGNED_BYTE);
    a_tfm -> unpacked_widths = NEW (a_tfm->bc+num_chars, fixword);
    a_tfm -> unpacked_heights = NEW (a_tfm->bc+num_chars, fixword);
    a_tfm -> unpacked_depths = NEW (a_tfm->bc+num_chars, fixword);
  }
  for (i=0; i<num_chars; i++) {
    (a_tfm->width_index)[i] = get_unsigned_pair (tfm_file);
    (a_tfm->height_index)[i] = get_unsigned_byte (tfm_file);
    (a_tfm->depth_index)[i] = get_unsigned_byte (tfm_file);
    /* Ignore remaining quad */
    get_unsigned_quad (tfm_file);
  }
}

static void do_ofm_one_char_info (FILE *tfm_file, struct a_tfm *a_tfm)
{
  UNSIGNED_QUAD i;
  UNSIGNED_QUAD num_char_infos, char_infos_read;
  UNSIGNED_QUAD num_chars;
  UNSIGNED_QUAD char_info_size;
  char_info_size = 3 + (a_tfm->npc/2);
  num_char_infos = (a_tfm -> ncw) / char_info_size;
  num_chars = (a_tfm->ec - a_tfm ->bc) + 1;
#ifdef DEBUG
  fprintf (stderr, "\nReading %ld level 1 chars\n", num_chars);
#endif
  if (num_chars != 0) {
    a_tfm -> width_index = NEW (num_chars, UNSIGNED_PAIR);
    a_tfm -> height_index = NEW (num_chars, UNSIGNED_BYTE);
    a_tfm -> depth_index = NEW (num_chars, UNSIGNED_BYTE);
    a_tfm -> unpacked_widths = NEW (a_tfm->bc+num_chars, fixword);
    a_tfm -> unpacked_heights = NEW (a_tfm->bc+num_chars, fixword);
    a_tfm -> unpacked_depths = NEW (a_tfm->bc+num_chars, fixword);
  }
  for (i=0, char_infos_read = 0; i<num_chars && char_infos_read < num_char_infos; i++) {
    int repeats, j;
    (a_tfm->width_index)[i] = get_unsigned_pair (tfm_file);
    (a_tfm->height_index)[i] = get_unsigned_byte (tfm_file);
    (a_tfm->depth_index)[i] = get_unsigned_byte (tfm_file);
    /* Ignore next quad */
    get_unsigned_quad (tfm_file);
    repeats = get_unsigned_pair (tfm_file);
    /* Skip params */
    for (j=0; j<a_tfm->npc; j++) {
      get_unsigned_pair (tfm_file);
    }
    /* Remove word padding if necessary */
    if (ISEVEN (a_tfm->npc)){
      get_unsigned_pair (tfm_file);
    }
    char_infos_read += 1;
    if (i+repeats > num_chars)
      ERROR ("repeats causes number of characters to be exceeded");
    for (j=0; j<repeats; j++) {
      a_tfm->width_index[i+j+1] = a_tfm->width_index[i];
      a_tfm->height_index[i+j+1] = a_tfm->height_index[i];
      a_tfm->depth_index[i+j+1] = a_tfm->depth_index[i];
    }
    /* Skip ahead because we have already handled repeats */
    i += repeats;
  }
#ifdef DEBUG
  fprintf (stderr, "\npackets read = %ld/%ld\n", char_infos_read, num_char_infos);
  fprintf (stderr, "\ncharacters defined = %ld/%ld\n", i, num_chars);
#endif
}

static void ofm_unpack_arrays (struct a_tfm *a_tfm, UNSIGNED_QUAD num_chars)
{
  unsigned i;
  for (i=0; i<num_chars; i++) {
    (a_tfm->unpacked_widths)[a_tfm->bc+i] = (a_tfm->width)[(a_tfm->width_index)[i]];
    (a_tfm->unpacked_heights)[a_tfm->bc+i] = (a_tfm->height)[(a_tfm->height_index)[i]];
    (a_tfm->unpacked_depths)[a_tfm->bc+i] = (a_tfm->depth)[(a_tfm->depth_index)[i]];
  }
}

static void ofm_get_arrays (FILE *tfm_file, struct a_tfm *a_tfm, int level)
{
  switch (level) {
  case 0:
#ifdef DEBUG
    fprintf (stderr, "Reading %ld word header\n", a_tfm->wlenheader);
#endif
    do_fix_word_array (tfm_file, &(a_tfm -> header), a_tfm -> wlenheader);
#ifdef DEBUG
    fprintf (stderr, "Reading %ld char_infos\n", (a_tfm->ec)-(a_tfm->bc)+1);
#endif
    do_ofm_zero_char_info (tfm_file, a_tfm);
    break;
  case 1:
#ifdef DEBUG
    fprintf (stderr, "Reading %ld char_infos words\n", a_tfm->ncw);
#endif
    do_ofm_one_char_info (tfm_file, a_tfm);
    break;
  default:
    ERROR ("level != 0 or 1 in ofm_get_arrays()");
  }
#ifdef DEBUG
  fprintf (stderr, "Reading %ld widths\n", a_tfm -> nwidths);
#endif
  do_fix_word_array (tfm_file, &(a_tfm -> width), a_tfm -> nwidths);
#ifdef DEBUG
  fprintf (stderr, "Reading %ld heights\n", a_tfm -> nheights);
#endif
  do_fix_word_array (tfm_file, &(a_tfm -> height), a_tfm -> nheights);
#ifdef DEBUG
  fprintf (stderr, "Reading %ld depths\n", a_tfm -> ndepths);
#endif
  do_fix_word_array (tfm_file, &(a_tfm -> depth), a_tfm -> ndepths);
  ofm_unpack_arrays (a_tfm, (a_tfm->ec)-(a_tfm->bc)+1);
  return;
}

static void get_ofm (FILE *ofm_file, UNSIGNED_QUAD ofm_file_size,
		     struct a_tfm *a_tfm)
{
  int level;
  level = ofm_get_sizes (ofm_file, ofm_file_size, a_tfm);
  ofm_get_arrays (ofm_file, a_tfm, level);
  return;
}

static void get_tfm (FILE *tfm_file, UNSIGNED_QUAD tfm_file_size,
		     struct a_tfm *a_tfm)
{
  get_sizes (tfm_file, tfm_file_size, a_tfm);
  get_arrays (tfm_file, a_tfm);
  return;
}

/* External Routine */
#define TFM_FORMAT 1
#define OFM_FORMAT 2

int tfm_open (const char *tfm_name, int must_exist)
{
  FILE *tfm_file;
  int i, format;
  UNSIGNED_QUAD tfm_file_size;
  char *file_name;

  for (i = 0; i < numtfms; i++) {
    if (!strcmp(tfm_name, tfm[i].tex_name))
      return i;
  }

  /* The procedure to search tfm or ofm files:
     1. Search tfm file with the given name with the must_exist flag unset.
     2. Search ofm file with the given name with the must_exist flag unset.
     3. If not found and must_exist flag is set, try again to search
        tfm file with the must_exist flag set.
     4. If not found and must_exist flag is not set, return -1. */

  if (file_name = kpse_find_file(tfm_name, kpse_tfm_format, 0))
    format = TFM_FORMAT;
  else if (file_name = kpse_find_file(tfm_name, kpse_ofm_format, 0))
    format = OFM_FORMAT;
  /* In case that must_exist is set, MiKTeX returns always non-NULL value
     even if the tfm file is not found. */
  else if (must_exist) {
    if (file_name = kpse_find_file(tfm_name, kpse_tfm_format, 1))
      format = TFM_FORMAT;
    else {
      fprintf(stderr, "\n** Fatal: Unable to find TFM file '%s'.", tfm_name);
      ERROR ("\n");
    }
  } else
    return -1;

  tfms_need(numtfms + 1);
  a_tfm_init(tfm + numtfms);

  if (!(tfm_file = MFOPEN (file_name, FOPEN_RBIN_MODE))) {
    fprintf(stderr, "\n** Fatal: Specified TFM/OFM file '%s' ", tfm_name);
    ERROR ("cannot be opened.\n");
  }

  if (verbose == 1)
    fprintf(stderr, "(TFM/OFM:%s", tfm_name);
  if (verbose > 1)
    fprintf(stderr, "(TFM/OFM:%s", file_name);

  if ((tfm_file_size = file_size(tfm_file)) < 24) {
    ERROR ("\n** Fatal: TFM/OFM file too small to be a valid file.\n");
  }

  if (format == OFM_FORMAT)
    get_ofm(tfm_file, tfm_file_size, &tfm[numtfms]);
  else
    get_tfm(tfm_file, tfm_file_size, &tfm[numtfms]);

  tfm[numtfms].tex_name = NEW (strlen(tfm_name)+1, char);
  strcpy(tfm[numtfms].tex_name, tfm_name);
  MFCLOSE (tfm_file);
  if (verbose) 
    fprintf(stderr, ")");
  if (verbose > 3)
    dump_sizes(&tfm[numtfms]);

  return numtfms++;
}

void tfm_close_all(void)
{
  int i;
  for (i=0; i<numtfms; i++) {
    if (tfm[i].header)
      RELEASE (tfm[i].header);
    if (tfm[i].char_info)
      RELEASE (tfm[i].char_info);
    if (tfm[i].width)
      RELEASE (tfm[i].width);
    if (tfm[i].height)
      RELEASE (tfm[i].height);
    if (tfm[i].depth)
      RELEASE (tfm[i].depth);
    RELEASE (tfm[i].tex_name);
    RELEASE (tfm[i].unpacked_widths);
    RELEASE (tfm[i].unpacked_heights);
    RELEASE (tfm[i].unpacked_depths);
    if (tfm[i].chartypes)
      RELEASE (tfm[i].chartypes);
    if (tfm[i].width_index)
      RELEASE (tfm[i].width_index);
    if (tfm[i].height_index)
      RELEASE (tfm[i].height_index);
    if (tfm[i].depth_index)
      RELEASE (tfm[i].depth_index);
  }
  if (tfm)
    RELEASE (tfm);
}

/* tfm_get_width returns the width of the font
   as a (double) fraction of the design size */
double tfm_get_width (int font_id, UNSIGNED_QUAD ch)
{
  if (tfm[font_id].id == JFM_ID || tfm[font_id].id == JFMV_ID )
    ch = (tfm[font_id].chartypes)[ch];
  if (tfm[font_id].unpacked_widths && ch <= tfm[font_id].ec)
    return (double) (tfm[font_id].unpacked_widths)[ch] / FWBASE;
  else return 0.0;
}

double tfm_get_height (int font_id, UNSIGNED_QUAD ch)
{
  if (tfm[font_id].id == JFM_ID || tfm[font_id].id == JFMV_ID )
    ch = (tfm[font_id].chartypes)[ch];
  if (tfm[font_id].unpacked_heights && ch <= tfm[font_id].ec)
    return (double) (tfm[font_id].unpacked_heights)[ch] / FWBASE;
  else return 0.0;
}

double tfm_get_depth (int font_id, UNSIGNED_QUAD ch)
{
  if (tfm[font_id].id == JFM_ID || tfm[font_id].id == JFMV_ID )
    ch = (tfm[font_id].chartypes)[ch];
  if (tfm[font_id].unpacked_depths && ch <= tfm[font_id].ec)
    return (tfm[font_id].unpacked_depths)[ch]/FWBASE;
  else return 0.0;
}

fixword tfm_get_fw_width (int font_id, UNSIGNED_QUAD ch)
{
  if (tfm[font_id].id == JFM_ID || tfm[font_id].id == JFMV_ID )
    ch = (tfm[font_id].chartypes)[ch];
  if (tfm[font_id].unpacked_widths && ch <= tfm[font_id].ec) {
    return (tfm[font_id].unpacked_widths)[ch];
  }
  return 0;
}

fixword tfm_get_fw_height (int font_id, UNSIGNED_QUAD ch)
{
  if (tfm[font_id].id == JFM_ID || tfm[font_id].id == JFMV_ID )
    ch = (tfm[font_id].chartypes)[ch];
  if (tfm[font_id].unpacked_heights && ch <= tfm[font_id].ec)
    return (tfm[font_id].unpacked_heights)[ch];
  return 0;
}

fixword tfm_get_fw_depth (int font_id, UNSIGNED_QUAD ch)
{
  if (tfm[font_id].id == JFM_ID || tfm[font_id].id == JFMV_ID )
    ch = (tfm[font_id].chartypes)[ch];
  if (tfm[font_id].unpacked_depths && ch <= tfm[font_id].ec)
    return (tfm[font_id].unpacked_depths)[ch];
  return 0;
}

fixword tfm_string_width (int font_id, unsigned char *s, unsigned len)
{
  fixword result = 0;
  unsigned i;
  if (tfm[font_id].unpacked_widths) 
    for (i=0; i<len; i++) {
      if (s[i] <= tfm[font_id].ec)
	result += tfm[font_id].unpacked_widths[s[i]];
    }
  return result;
}

fixword tfm_string_depth (int font_id, unsigned char *s, unsigned len)
{
  fixword result = 0;
  unsigned i;
  if (tfm[font_id].unpacked_depths) 
    for (i=0; i<len; i++) {
      if (s[i] <= tfm[font_id].ec)
	result = MAX(result, tfm[font_id].unpacked_depths[s[i]]);
    }
  return result;
}

fixword tfm_string_height (int font_id, unsigned char *s, unsigned len)
{
  fixword result = 0;
  unsigned i;
  if (tfm[font_id].unpacked_heights) 
    for (i=0; i<len; i++) {
      if (s[i] <= tfm[font_id].ec)
	result = MAX(result, tfm[font_id].unpacked_heights[s[i]-tfm[font_id].bc]);
    }
  return result;
}

UNSIGNED_PAIR tfm_get_firstchar (int font_id)
{
  return tfm[font_id].bc;
}

UNSIGNED_PAIR tfm_get_lastchar (int font_id)
{
  return tfm[font_id].ec;
}

double tfm_get_design_size (int font_id)
{
  return ((tfm[font_id].header))[1]/FWBASE*(72.0/72.27);
}


double tfm_get_max_width (int font_id)
{
  SIGNED_QUAD max = 0;
  int i;
  for (i=0; i<tfm[font_id].nwidths; i++) {
    if ((tfm[font_id].width)[i] > max)
      max = (tfm[font_id].width)[i];
  }
  return (max/FWBASE);
}

int tfm_is_fixed_width (int font_id)
{
  /* We always have two widths since width[0] = 0.
     A fixed width font will have width[1] = something
     and not have any other widths */
  return (tfm[font_id].nwidths == 2);
}

double tfm_get_max_height (int font_id)
{
  SIGNED_QUAD max = 0;
  int i;
  for (i=0; i<tfm[font_id].nheights; i++) {
    if ((tfm[font_id].height)[i] > max)
      max = (tfm[font_id].height)[i];
  }
  return (max/FWBASE);
}

double tfm_get_max_depth (int font_id)
{
  SIGNED_QUAD max = 0;
  int i;
  for (i=0; i<tfm[font_id].ndepths; i++) {
    if ((tfm[font_id].depth)[i] > max)
      max = (tfm[font_id].depth)[i];
  }
  return (max/FWBASE);
}

char is_jfm (int font_id)
{
  return (tfm[font_id].id == JFM_ID || tfm[font_id].id == JFMV_ID) ? 1 : 0;
}

char is_vertical (int font_id) /* Vertical version of JFM */
{
  return (tfm[font_id].id == JFMV_ID) ? 1 : 0;
}
