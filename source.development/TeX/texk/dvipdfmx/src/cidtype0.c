/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/cidtype0.c,v 1.1 2004/01/08 13:44:13 rahtz Exp $
    
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
  CID-Keyed Font support:
   Only CFF/OpenType CID-Keyed Font with Type 2 charstrings is supported.

  TODO:
   Widths from TFM ?, from Charstring ?, from OpenType table ?
*/ 

#include <string.h>

#include "system.h"
#include "mem.h"
#include "mfileio.h"
#include "error.h"

#include "pdfobj.h"

#include "cmap.h"
#include "type0.h"
#include "cid.h"

/* font info. from OpenType tables */
#include "sfnt.h"
#include "tt_aux.h"

#include "cff_types.h"
#include "cff_limits.h"
#include "cff.h"
#include "cff_dict.h"
#include "cs_type2.h"

#include "cidtype0.h"

#ifdef WIN32
#ifdef kpse_program_binary_format
#undef kpse_program_binary_format
#define kpse_program_binary_format kpse_program_text_format
#endif
#endif /* WIN32 */

#ifndef kpse_opentype_format
#define kpse_opentype_format kpse_program_binary_format
#endif

static unsigned char verbose = 0;

void cidtype0_set_verbose(void)
{
  if (verbose < 255) verbose++;
}

void do_cidtype0 (int cid_id)
{
  struct cid_font_s *cidfont;
  pdf_obj *stream_dict;
  sfnt *sfont;
  cff_font *cff;
  cff_index *charstrings, *topdict, *fdarray, *private, *idx;
  cff_charsets *charset = NULL;
  cff_fdselect *fdselect = NULL;
  long topdict_offset, fdarray_offset;
  long charstring_len, max_len;
  long destlen = 0;
  card8 *dest, *data;
  unsigned short num_glyphs, max_glyphs, cid;
  card16 cid_count, cs_count;
  int parent_id, face, fd, prev_fd;
  char *shortname, *fullname;
  char *used_chars;
  long size, offset;
  int i;

  cidfont = cid_this_font(cid_id);

  pdf_release_obj(cidfont->fontdict);
  pdf_release_obj(cidfont->descriptor);
  cidfont->descriptor = NULL;
  cidfont->fontdict = NULL;

  if (cidfont->embedd == 0) {
    return; /* no embedd */
  }

  face = cidfont->index;
  shortname = cidfont->filename;

  fullname = kpse_find_file(shortname, kpse_opentype_format, 1);
  if (fullname == NULL) {
    fprintf (stderr, "Unable to find or open OT file (%s)", shortname);
    ERROR ("This existed when I checked it earlier!");
  }

  if (verbose == 1) {
    fprintf(stderr, "%s", cidfont->ident);
  } else if (verbose > 1) {
    fprintf(stderr, "%s", fullname);
  }

  if ((parent_id = (cidfont->parent)[0]) < 0 &&
      (parent_id = (cidfont->parent)[1]) < 0) {
    ERROR("No parent Type 0 font !");
  }

  if ((used_chars = type0_font_used(parent_id)) == NULL) {
    ERROR("Cannot create embeddable font without used_chars");
  }

  if ((sfont = sfnt_open(fullname)) == NULL)
    ERROR("cannot open OpenType font file");

  if (sfnt_read_table_directory(sfont, 0) < 0)
    ERROR("cannot read OpenType table directory");

  if (sfont->type != SFNT_TYPE_POSTSCRIPT ||
      (offset = sfnt_find_table_pos(sfont, "CFF ")) == 0) {
    ERROR("not a CFF/OpenType font ?");
  }

  if ((cff = cff_open(sfont->stream, offset, face)) == NULL) {
    ERROR("Cannot open CFF font");
  }

  if (!(cff->flag & FONTTYPE_CIDFONT))
    ERROR("not a CIDFont");

  /* Offsets from DICTs */
  cff_read_charsets(cff);
  cff_read_fdselect(cff);
  cff_read_fdarray(cff);
  cff_read_private(cff);

  cff_read_subrs(cff);

  offset = (long) cff_dict_get(cff->topdict, "CharStrings", 0);
  cff_seek_set(cff, offset);
  idx = cff_get_index_header(cff);
  /* offset is now absolute offset ... bad */
  offset = ftell(cff->stream);
  
  if ((cs_count = idx->count) < 2) {
    ERROR("No valid charstring data found.");
  }

#define MAX_GLYPHS 256
  max_glyphs = MAX_GLYPHS;

  /* New Charsets data */
  charset = NEW(1, cff_charsets);
  charset->format = 0;
  charset->num_entries = 0;
  charset->data.glyphs = NEW(max_glyphs, s_SID);

  /* New FDSelect data */
  fdselect = NEW(1, cff_fdselect);
  fdselect->format = 3;
  fdselect->num_entries = 0;
  fdselect->data.ranges = NEW(max_glyphs, cff_range3);

  /* New CharStrings INDEX */
  charstrings = cff_new_index(max_glyphs);
  max_len = 2 * CS_STR_LEN_MAX;
  charstrings->data = NEW(max_len, card8);
  charstring_len = 0;

  if (cff_dict_known(cff->topdict, "CIDCount")) {
    cid_count = (card16) cff_dict_get(cff->topdict, "CIDCount", 0);
  } else {
    cid_count = CFF_CIDCOUNT_DEFAULT;
  }

  num_glyphs = 0;
  prev_fd = -1;

  add_to_used_chars2(used_chars, 0); /* .notdef */

  data = NEW(CS_STR_LEN_MAX, card8);
  for (cid=0;cid<0xffffu;cid++) {
    if (is_used_char2(used_chars, cid)) {
      unsigned short gid;

      if (cid > cid_count - 1) {
	fprintf(stderr, "\n** Error: Invalid CID %u (CIDCount %u) **\n",
		cid, cid_count);
	ERROR("Invalid CID");
      }

      gid = cff_charsets_lookup(cff, cid);

      if (cid > 0 && gid == 0) {
	fprintf(stderr, "\n** Warning: Missing character: %u **\n", cid);
      }

      if ((size = (idx->offset)[gid+1] - (idx->offset)[gid])
	  > CS_STR_LEN_MAX) {
	fprintf(stderr, "\n** Error: Charstring too long: %u **\n", gid);
	ERROR("Corrupt CID font file ?");
      }

      if (charstring_len + CS_STR_LEN_MAX >= max_len) {
	max_len = charstring_len + 2 * CS_STR_LEN_MAX;
	charstrings->data = RENEW(charstrings->data, max_len, card8);
      }
      if (num_glyphs >= max_glyphs) {
	max_glyphs += MAX_GLYPHS;
	charstrings->offset = RENEW(charstrings->offset,
				    max_glyphs+1, l_offset);
	charset->data.glyphs = RENEW(charset->data.glyphs, max_glyphs, s_SID);
	fdselect->data.ranges = RENEW(fdselect->data.ranges,
				      max_glyphs, cff_range3);
      }

      (charstrings->offset)[num_glyphs] = charstring_len + 1;
      seek_absolute(cff->stream, offset + (idx->offset)[gid] - 1);
      fread(data, 1, size, cff->stream);
      fd = cff_fdselect_lookup(cff, gid);
      charstring_len += cs_copy_charstring(charstrings->data + charstring_len,
					   max_len - charstring_len,
					   data, size,
					   cff->gsubr, (cff->subrs)[fd]);
      if (cid > 0 && gid > 0) {
	charset->data.glyphs[charset->num_entries] = cid;
	charset->num_entries += 1;
      }
      if (fd != prev_fd) {
	fdselect->data.ranges[fdselect->num_entries].first = num_glyphs;
	fdselect->data.ranges[fdselect->num_entries].fd = fd;
	fdselect->num_entries += 1;
	prev_fd = fd;
      }
      num_glyphs++;
    }
  }
  RELEASE(data);
  cff_release_index(idx);
  
  (charstrings->offset)[num_glyphs] = charstring_len + 1;
  charstrings->count = num_glyphs;
  charstring_len = cff_index_size(charstrings);
  cff->num_glyphs = num_glyphs;

  /* discard old one, set new data */
  cff_release_charsets(cff->charsets);
  cff->charsets = charset;
  cff_release_fdselect(cff->fdselect);
  cff->fdselect = fdselect;

  /* no Global subr */
  if (cff->gsubr) {
    cff_release_index(cff->gsubr);
  }
  cff->gsubr = cff_new_index(0);

  /*  DICT sizes (offset set to long int) */
  topdict = cff_new_index(1);
  fdarray = cff_new_index(cff->num_fds);
  private = cff_new_index(cff->num_fds);
  cff_dict_remove(cff->topdict, "Private");  /* some bad font may have */
  cff_dict_remove(cff->topdict, "Encoding"); /* some bad font may have */
  (topdict->offset)[1] = cff_dict_pack(cff->topdict,
				       (card8 *) work_buffer,
				       WORK_BUFFER_SIZE) + 1;
  for (i=0;i<cff->num_fds;i++) {
    if (cff->subrs[i]) {
      cff_release_index(cff->subrs[i]);
      cff->subrs[i] = NULL;
    }
    size = 0;
    if ((cff->private)[i]) {
      cff_dict_remove((cff->private)[i], "Subrs"); /* no Subrs */
      size = cff_dict_pack((cff->private)[i],
			   (card8 *) work_buffer, WORK_BUFFER_SIZE);
      if (size < 1) { /* Private had contained only Subr */
	cff_dict_remove((cff->fdarray)[i], "Private");
      }
    }
    (private->offset)[i+1] = (private->offset)[i] + size;
    (fdarray->offset)[i+1] = (fdarray->offset)[i] +
      cff_dict_pack((cff->fdarray)[i],
		    (card8 *) work_buffer, WORK_BUFFER_SIZE);
  }

  destlen = 4; /* header size */
  destlen += cff_set_name(cff, cidfont->fontname);
  destlen += cff_index_size(topdict);
  destlen += cff_index_size(cff->string);
  destlen += cff_index_size(cff->gsubr);
  destlen += (charset->num_entries) * 2 + 1; /* charset format 0 */
  destlen += (fdselect->num_entries) * 3 + 5; /* fdselect format 3 */
  destlen += charstring_len;
  destlen += cff_index_size(fdarray);
  destlen += (private->offset)[private->count] - 1; /* Private is not INDEX */

  dest = NEW(destlen, card8);

  offset = 0;
  /* Header */
  offset += cff_put_header(cff, dest + offset, destlen - offset);
  /* Name */
  offset += cff_pack_index(cff->name, dest + offset, destlen - offset);
  /* Top DICT */
  topdict_offset = offset;
  offset += cff_index_size(topdict);
  /* Strings */
  offset += cff_pack_index(cff->string, dest + offset, destlen - offset);
  /* Global Subrs */
  offset += cff_pack_index(cff->gsubr, dest + offset, destlen - offset);

  /* charset */
  cff_dict_set(cff->topdict, "charset", 0, offset);
  offset += cff_pack_charsets(cff, dest + offset, destlen - offset);

  /* FDSelect */
  cff_dict_set(cff->topdict, "FDSelect", 0, offset);
  offset += cff_pack_fdselect(cff, dest + offset, destlen - offset);

  /* CharStrings */
  cff_dict_set(cff->topdict, "CharStrings", 0, offset);
  offset += cff_pack_index(charstrings, dest + offset, charstring_len);
  cff_release_index(charstrings);

  /* FDArray and Private */
  cff_dict_set(cff->topdict, "FDArray", 0, offset);
  fdarray_offset = offset;
  offset += cff_index_size(fdarray);

  fdarray->data = NEW(fdarray->offset[fdarray->count] - 1, card8);
  for (i=0;i<cff->num_fds;i++) {
    size = (private->offset)[i+1] - (private->offset)[i];
    if ((cff->private)[i] && size > 0) {
      cff_dict_pack((cff->private)[i], dest + offset, size);
      cff_dict_set((cff->fdarray)[i], "Private", 0, size);
      cff_dict_set((cff->fdarray)[i], "Private", 1, offset);
    }
    cff_dict_pack((cff->fdarray)[i],
		  fdarray->data + (fdarray->offset)[i] - 1,
		  fdarray->offset[fdarray->count] - 1);
    offset += size;
  }
  cff_pack_index(fdarray, dest + fdarray_offset, cff_index_size(fdarray));
  cff_release_index(fdarray);
  cff_release_index(private);

  /* finally Top DICT */
  topdict->data = NEW(topdict->offset[topdict->count] - 1, card8);
  cff_dict_pack(cff->topdict,
		topdict->data, topdict->offset[topdict->count] - 1);
  cff_pack_index(topdict, dest + topdict_offset, cff_index_size(topdict));
  cff_release_index(topdict);

  /* Copyright and Trademark Notice */

  cff_close(cff);
  sfnt_close(sfont);

  if (verbose > 1) {
    fprintf(stderr, "\n  Embedding %u of %u glyphs\n", num_glyphs, cs_count);
    fprintf(stderr, "  Embedded size: %ld bytes\n", offset);
  }

  /* W */
  if (cidfont->w_array) {
    pdf_release_obj(cidfont->w_array);
    cidfont->w_array = NULL;
  }

  /* CIDSet */
  pdf_add_stream (cidfont->cidset, used_chars, 8192);
  pdf_release_obj(cidfont->cidset);
  cidfont->cidset = NULL;

  /* FontFile */
  stream_dict = pdf_stream_dict(cidfont->fontfile);
  pdf_add_dict(stream_dict, pdf_new_name("Subtype"),
	       pdf_new_name("CIDFontType0C"));
  pdf_add_stream(cidfont->fontfile, (char *) dest, offset);
  pdf_release_obj(cidfont->fontfile);
  cidfont->fontfile = NULL;
  RELEASE(dest);

  return;
}

int cidtype0_get_id (const char *map_name, int cmap_id)
{
  struct cid_font_s *cidfont;
  struct cid_sysinfo_s *ros, *cmap_ros;
  sfnt *sfont;
  cff_font *cff;
  char *shortname, *fullname, *fontname, *short_fontname, *attribname;
  unsigned long offset = 0;
  int face = 0, embedd = 1;

  shortname = NEW(strlen(map_name)+6, char); /* .otf and one indication blank */
  strcpy(shortname+1, map_name);
  *shortname = ' '; /* indication blank */

  if ((face = split_face(shortname, &attribname, &embedd)) < 0) {
    if (shortname) RELEASE(shortname);
    return -1;
  }

  if (strstr(shortname, ".otf") == NULL) {
    if (attribname) {
      memmove(attribname+4, attribname, strlen(attribname)+1);
      attribname += 4;
    }
    strcat(shortname, ".otf");
  }

  if ((fullname = kpse_find_file(shortname, kpse_opentype_format, 1))
      == NULL) {
    RELEASE(shortname);
    return -1;
  }

  if ((sfont = sfnt_open(fullname)) == NULL) {
    fprintf(stderr, "not an OTF file or file may corrupt\n");
    RELEASE(shortname);
    return -1;
  }

  if (sfont->type != SFNT_TYPE_POSTSCRIPT) {
    RELEASE(shortname);
    sfnt_close(sfont);
    return -1;
  }

  if (sfnt_read_table_directory(sfont, 0) < 0 ||
      (offset = sfnt_find_table_pos(sfont, "CFF ")) == 0) {
    RELEASE(shortname);
    sfnt_close(sfont);
    return -1;
  }

  if ((cff = cff_open(sfont->stream, offset, face)) == NULL) {
    ERROR("Cannot read CFF font data");
  }

  if (!(cff->flag & FONTTYPE_CIDFONT)) {
    RELEASE(shortname);
    cff_close(cff);
    sfnt_close(sfont);
    return -1;
  }

  ros = NEW(1, struct cid_sysinfo_s);
  ros->registry =
    cff_get_string(cff, (s_SID)cff_dict_get(cff->topdict, "ROS", 0));
  ros->ordering =
    cff_get_string(cff, (s_SID)cff_dict_get(cff->topdict, "ROS", 1));
  ros->supplement = (int)cff_dict_get(cff->topdict, "ROS", 2);

  cmap_ros = cmap_sysinfo(cmap_id);
  if (strcmp(ros->registry, cmap_ros->registry) != 0 ||
      strcmp(ros->ordering, cmap_ros->ordering) != 0) {

    if (verbose) {
      fprintf(stderr, "\nCharacter collection mismatched:\n");
      fprintf(stderr, " Font: %s-%s-%d\n",
	      ros->registry, ros->ordering, ros->supplement);
      fprintf(stderr, " CMap: %s-%s-%d\n",
	      cmap_ros->registry, cmap_ros->ordering, cmap_ros->supplement);
    }

    RELEASE(ros->registry);
    RELEASE(ros->ordering);
    RELEASE(ros);
    RELEASE(shortname);
    cff_close(cff);
    sfnt_close(sfont);
    return -1;
  }

  if (ros->supplement < cmap_ros->supplement) {
    fprintf(stderr, "\n*** Warning: CMap has higher supplmement ***\n");
  }

  if ((short_fontname = cff_get_name(cff)) == NULL) {
    ERROR("No valid fontname found");
  }

  cff_close(cff);

  /*
    mangled name requires more 7 bytes.
    ',BoldItalic' requires more 11 bytes.
  */
  fontname = NEW(strlen(short_fontname)+19, char);
  strcpy(fontname, short_fontname);
  RELEASE(short_fontname);

  if (attribname) strcat(fontname, attribname);

  cidfont = cid_new_font(CIDFONT_TYPE0, ros);

  RELEASE(ros->registry);
  RELEASE(ros->ordering);
  RELEASE(ros);

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
		pdf_new_name ("CIDFontType0"));

  /* getting font info. from TrueType tables */
  if ((cidfont->descriptor
       = tt_get_fontdesc(sfont, &embedd, 0)) == NULL) {
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
  cid_add_sysinfo(cidfont->fontdict, cidfont->sysinfo);
  pdf_add_dict (cidfont->fontdict, 
		pdf_new_name ("FontDescriptor"),
		pdf_link_obj(pdf_ref_obj(cidfont->descriptor)));
  pdf_add_dict (cidfont->fontdict, 
		pdf_new_name ("DW"),
		pdf_new_number (1000)); /* not sure */

  if (cidfont->embedd) {
    cidfont->w_array = NULL; /* not supported yet */
    cidfont->cidset = pdf_new_stream(STREAM_COMPRESS);
    pdf_add_dict (cidfont->descriptor, pdf_new_name("CIDSet"),
		  pdf_link_obj(pdf_ref_obj(cidfont->cidset)));
    cidfont->fontfile = pdf_new_stream(STREAM_COMPRESS);
    pdf_add_dict (cidfont->descriptor, pdf_new_name("FontFile3"),
		  pdf_link_obj(pdf_ref_obj(cidfont->fontfile)));
  } else {
    cidfont->fontfile = NULL;
    cidfont->cidset = NULL;
  }

  sfnt_close(sfont);

  return 0; /* success */
}

void cidtype0_release(int cid_id)
{
  /* do nothing */
  return;
}
