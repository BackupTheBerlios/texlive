/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/tt_build.c,v 1.1 2004/01/08 13:44:15 rahtz Exp $
    
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
  TrueType glyf, loca, hmtx, ... table build procedures
*/

#include <stdio.h>

#include "system.h"
#include "mem.h"
#include "error.h"
#include "sfnt.h"
#include "tt_table.h"
#include "tt_build.h"

/*
  number of glyphs are limited to NUM_GLYPH_LIMIT.
  average Japanese users may use thousands of characters.
*/
#define NUM_GLYPH_LIMIT 20480
/*
  xxx_table_data grows GROW_AMOUNT bytes each time if more space is needed.
*/
#define GROW_AMOUNT 40960
/*
  used_glyphs array grows MAX_GLYPHS units each time if more space is needed.
*/
#define MAX_GLYPHS 256

/* used by qsort to sort used_glyphs array in ascending newgid order */
int CDECL nidx_cmp (const void *v1, const void *v2)
{
  struct glyph_desc *sv1 = (struct glyph_desc *) v1;
  struct glyph_desc *sv2 = (struct glyph_desc *) v2;
  return sv1->newgid - sv2->newgid;
}

void tt_add_to_used (struct tt_glyphs *g, USHORT gid, USHORT new_gid)
{
  USHORT i;

  for (i=0;i<(g->num_glyphs);i++) {
    if (new_gid == (g->used_glyphs)[i].newgid) {
      if (gid == (g->used_glyphs)[i].gid) { /* already registered */
	return;
      } else {
	fprintf(stderr, "\n** Notice: id %u not available ", new_gid);
	fprintf(stderr, "(used by glyph 0x%04x), ", (g->used_glyphs)[i].gid);
	fprintf(stderr, "glyph 0x%04x lost **\n", gid);
	return;
      }
    } /* two (or more) distinct id's can be asigned to a single glyph */
  } /* end for */
  
  if (g->num_glyphs >= NUM_GLYPH_LIMIT) {
    fprintf(stderr, "%u >= %u\n", g->num_glyphs, NUM_GLYPH_LIMIT);
    ERROR("Too many glyphs");
  }

  /* grow used_glyphs array */
  if (g->num_glyphs >= g->max_glyphs) {
    g->max_glyphs += MAX_GLYPHS;
    g->used_glyphs = RENEW(g->used_glyphs, g->max_glyphs, struct glyph_desc);
  }

  (g->used_glyphs)[g->num_glyphs].gid = gid;
  (g->used_glyphs)[g->num_glyphs].newgid = new_gid;
  /* initial values */
  (g->used_glyphs)[g->num_glyphs].offset = 0UL;
  (g->used_glyphs)[g->num_glyphs].width = 0;
  /*
    (g->used_glyphs)[g->num_glyphs].llx = 0;
    (g->used_glyphs)[g->num_glyphs].lly = 0;
    (g->used_glyphs)[g->num_glyphs].urx = 0;
    (g->used_glyphs)[g->num_glyphs].ury = 0;
  */
  g->num_glyphs += 1;

  if (new_gid > g->last_gid)
    g->last_gid = new_gid;

  return;
}

/*
  initialization: must be invoked first !
 */
struct tt_glyphs *tt_build_init (void)
{
  struct tt_glyphs *g;

  g = NEW(1, struct tt_glyphs);

  g->num_glyphs = 0;
  g->last_gid = 0;
  g->max_glyphs = 0;
  g->emsize = 1;
  g->used_glyphs = NULL;

  tt_add_to_used(g, 0, 0);

  return g;
}

/* just RELEASE */
void tt_build_finish (struct tt_glyphs *g)
{
  if (g) {
    if (g->used_glyphs)
      RELEASE(g->used_glyphs);
    RELEASE(g);
  }
}

/*
  inefficient but usable in most cases

  o if the glyph found in used_glyphs array, then returns newgid.
    otherwise, append it with newgid = last_gid + 1 to used_glyphs array.

*/
static USHORT map_gid (struct tt_glyphs *g, USHORT gid)
{
  USHORT i;
  for (i=0;i<(g->num_glyphs);i++) {
    if (gid == (g->used_glyphs)[i].gid) {
      return (g->used_glyphs)[i].newgid; /* already registered */
    }
  }

  tt_add_to_used(g, gid, (USHORT)((g->last_gid)+1)); /* last_gid increases here */

  /* do not do num_glyphs++ and last_gid++ */

  return (g->last_gid);
}

/* build hmtx and loca tables */
static int build_other (sfnt *sfont, struct tt_glyphs *g,
			USHORT num_hm, ULONG last_offset)
{
  char *hmtx_table_data = NULL, *loca_table_data = NULL;
  ULONG hmtx_table_size, loca_table_size;
  USHORT default_advw, advw, last_advw; /* advance width */
  SHORT default_lsb, lsb;               /* left side bearing width */
  ULONG hmtx_offset;
  LONG gap;
  USHORT old_gid, gid;
  /* temp */
  char *p = NULL, *q = NULL;
  ULONG offset;
  int i, j;


  hmtx_table_size = (g->last_gid + 1) * 4;  /* USHORT + SHORT */
  loca_table_size = (g->last_gid + 2) * 4;  /* ULONG, we use long format */

  hmtx_table_data = p = NEW(hmtx_table_size, char);
  loca_table_data = q = NEW(loca_table_size, char);

  hmtx_offset = sfnt_locate_table(sfont, "hmtx");
  default_advw = sfnt_get_ushort(sfont); /* read advw for gid = 0 */
  default_lsb = sfnt_get_short(sfont);   /* read lsb for gid = 0 */
  sfnt_seek_set(sfont, hmtx_offset+4*(num_hm-1));
  last_advw = sfnt_get_ushort(sfont);    /* read last defined advw */

  /*
    NOTE:

     o used_glyphs is sorted in increasing newgid order.

     o first entry of used_glyphs must be gid = 0 and newgid = 0

  */

  if ((g->used_glyphs)[0].gid != 0 || (g->used_glyphs)[0].newgid != 0) {
    ERROR("unexpected error: first entry not a missing glyph !");
  }

  /* gid = 0 (missing glyph) */
  gid = 0;
  gap = 0;
  offset = 0;
  p += sfnt_put_ushort(p, default_advw);
  p += sfnt_put_short(p, default_lsb);
  q += sfnt_put_ulong(q, offset);

  (g->used_glyphs)[0].width = default_advw;

  for (i=1;i<(g->num_glyphs);i++) {
    old_gid = (g->used_glyphs)[i].gid;
    gap = (g->used_glyphs)[i].newgid - gid;
    offset = (g->used_glyphs)[i].offset;

    /* fill gap with default/previous value */
    if (gap > 1) {
      for (j=1;j<gap;j++) {
	p += sfnt_put_ushort(p, default_advw);
	p += sfnt_put_short(p, default_lsb);
	q += sfnt_put_ulong(q, offset);
      }
    } else if (gap == 0) {
      ERROR("unexpected error: two glyphs shares same gid !");
    } else if (gap < 0) {
      ERROR("unexpacted error: array not sorted properly !");
    }

    /* advance width and left side bearing */
    if (old_gid < num_hm) {
      sfnt_seek_set(sfont, hmtx_offset+old_gid*4);
      advw = sfnt_get_ushort(sfont);
      lsb = sfnt_get_short(sfont);
    } else {
      advw = last_advw;
      sfnt_seek_set(sfont, hmtx_offset+4*num_hm+(old_gid-num_hm)*2);
      lsb = sfnt_get_short(sfont);
    }
    p += sfnt_put_ushort(p, advw);
    p += sfnt_put_short(p, lsb);
    q += sfnt_put_ulong(q, offset);

    (g->used_glyphs)[i].width = advw;
    gid = (g->used_glyphs)[i].newgid;
  } /* end for */

  q += sfnt_put_ulong(q, last_offset);

  /* do not free xxx_data, PUT_TABLE just copy adresses */
  sfnt_set_table(sfont, "hmtx", (char *) hmtx_table_data, hmtx_table_size);
  sfnt_set_table(sfont, "loca", (char *) loca_table_data, loca_table_size);

  return 0;
}

#define ARG_1_AND_2_ARE_WORDS     (1 << 0)
#define ARGS_ARE_XY_VALUES        (1 << 1)
#define ROUND_XY_TO_GRID          (1 << 2)
#define WE_HAVE_A_SCALE           (1 << 3)
#define RESERVED                  (1 << 4)
#define MORE_COMPONENT            (1 << 5)
#define WE_HAVE_AN_X_AND_Y_SCALE  (1 << 6)
#define WE_HAVE_A_TWO_BY_TWO      (1 << 7)
#define WE_HAVE_INSTRUCTIONS      (1 << 8)
#define USE_MY_METRICS            (1 << 9)

int tt_build_tables (sfnt *sfont, struct tt_glyphs *g)
{
  char *glyf_table_data = NULL;
  ULONG glyf_table_size, max_glyf_table_size;
  /* some information available from other TrueType table */
  struct tt_head_table *head = NULL;
  struct tt_hhea_table *hhea = NULL;
  struct tt_maxp_table *maxp = NULL;
  SHORT indexToLocFormat;
  USHORT numGlyphs, numHMetrics;
  /* temp */
  ULONG currpos, *location;
  ULONG offset;
  USHORT i;

  if (sfont == NULL || sfont->stream == NULL)
    ERROR("file not opened");

  if ((sfont->type & (SFNT_TYPE_TRUETYPE|SFNT_TYPE_TTC)) == 0)
    ERROR("tt_build_tables(): invalid font type");

  if (g->num_glyphs > NUM_GLYPH_LIMIT) {
    fprintf(stderr, "Too many glyphs\n");
    return -1;
  }

  /*
    NOTE:
     
     we must sort used_glyphs in ascending newgid order. (see bellow)

  */
  qsort(g->used_glyphs, g->num_glyphs, sizeof(struct glyph_desc), nidx_cmp);
  
  /* === read head === */
  head = tt_read_head_table(sfont);
  /*
    We will never calculate total checksum.
  */
  head->checkSumAdjustment = 0;
  g->emsize = head->unitsPerEm; /* em box size */

  /* === read hhea === */
  hhea = tt_read_hhea_table(sfont);
  if (hhea->metricDataFormat != 0) {
    if (head) RELEASE(head);
    if (hhea) RELEASE(hhea);
    fprintf(stderr, "Unknown metricDataFormat.\n");
    return -1;
  }
  numHMetrics = hhea->numberOfHMetrics;

  /* === read maxp === */
  maxp = tt_read_maxp_table(sfont);

  /* === read loca === */
  indexToLocFormat = head->indexToLocFormat; /* from head table */
  numGlyphs = maxp->numGlyphs; /* from maxp table */

  sfnt_locate_table(sfont, "loca");
  location = NEW(numGlyphs+1, ULONG);
  if (indexToLocFormat == 0) { /* short */
    for (i=0;i<numGlyphs+1;i++) {
      location[i] = 2*((ULONG) sfnt_get_ushort(sfont));
    }
  } else if (indexToLocFormat == 1) { /* long */
    for (i=0;i<numGlyphs+1;i++) {
      location[i] = sfnt_get_ulong(sfont);
    }
  } else { /* unrecognized format */
    if (head) RELEASE(head);
    if (hhea) RELEASE(hhea);
    if (maxp) RELEASE(maxp);
    if (location) RELEASE(location);
    fprintf(stderr, "unknown IndexToLocFormat\n");
    return -1;
  }

  /* === read glyf table === */

  offset = sfnt_locate_table(sfont, "glyf");

  /*
    NOTE:

     first allocate (MIN_GLYPH_LEN x num_glyphs) bytes of memory space.
     MIN_GLYPH_LEN is a minimal size of TrueType glyph data:
      each glyphs consumes at least 10 bytes (number_of_counter and
      BoundingBox)

  */

#define MIN_GLYPH_LEN 10
  max_glyf_table_size = (g->num_glyphs) * MIN_GLYPH_LEN;
  glyf_table_data = NEW(max_glyf_table_size, char);

  /*
    NOTE:

     o num_glyphs may grow when a composite glyph is encountered.
       a component of glyph required by a composite glyph is appended
       to used_glyphs array with gid = last_gid + 1 if it is not already
       registered in used_glyphs array. composite glyph are modified so as
       to correctly refer new gid of its components.

     o used_glyphs array is always sorted by increasing newgid order.

  */

  currpos = 0;
  for (i=0;i<NUM_GLYPH_LIMIT;i++) { /* build glyf table */
    USHORT gid;     /* old gid */
    ULONG loc, len; /* location and length of glyph with id = gid */
    /* points current position and beginning of glyph (output buffer) */
    char *p, *save;
    /* if number_of_counters < 0, then composite glyph */
    SHORT number_of_contours;
    int padlen = 0;

    if (i >= g->num_glyphs) /* finished */
      break;

    /* this glyph is mapped to used_glyphs[i].newgid */
    gid = (g->used_glyphs)[i].gid;

    if (gid >= numGlyphs) {
      fprintf(stderr, " ** Warning: ignoring invalid glyph");
      fprintf(stderr, " with gid %u >= numGlyphs **\n", gid);
      loc = 0;
      len = 0; /* just skip it */
    } else {
      loc = location[gid];
      len = location[gid+1] - loc;
    }

    (g->used_glyphs)[i].offset = currpos; /* new offset */

    if (len == 0) { /* does not contains any data */
      continue;
    } else if (len < MIN_GLYPH_LEN) {
      ERROR("glyph does not contains valid data");
    }
    padlen = 4 - (len % 4);

    /* need more space */
    if (max_glyf_table_size < currpos + len + padlen) { /* plus padding */
      if (len + padlen > GROW_AMOUNT) {
	max_glyf_table_size += len + padlen;
      } else {
	max_glyf_table_size += GROW_AMOUNT;
      }
      glyf_table_data = RENEW(glyf_table_data, max_glyf_table_size, char);
    }

    save = p = glyf_table_data+currpos;
    sfnt_seek_set(sfont, offset+loc);

    number_of_contours = sfnt_get_short(sfont);
    p += sfnt_put_short(p, number_of_contours);
    p += sfnt_read(p, 8, sfont); /* BoundingBox: FWord x 4 */
    if (number_of_contours < 0) { /* composite glyph */
      USHORT flags, cgid; /* flag, gid of a component */
      LONG diff = 0;

      /* TODO: may cause overflow */
      do {
	flags = sfnt_get_ushort(sfont);
	p += sfnt_put_ushort(p, flags);
	if ((cgid = sfnt_get_ushort(sfont)) >= numGlyphs) {
	  fprintf(stderr, "component gid %u >= numGlyphs\n", gid);
	  ERROR("stop");
	}
	p += sfnt_put_ushort(p, map_gid(g, cgid));

	if (flags & ARG_1_AND_2_ARE_WORDS) {
	  p += sfnt_read(p, 4, sfont);
	} else {
	  p += sfnt_read(p, 2, sfont);
	}
	if (flags & WE_HAVE_A_SCALE) {
	  p += sfnt_read(p, 2, sfont); /* F2Dot14 */
	} else if (flags & WE_HAVE_AN_X_AND_Y_SCALE) {
	  p += sfnt_read(p, 4, sfont); /* F2Dot14 x 2 */
	} else if (flags & WE_HAVE_A_TWO_BY_TWO) {
	  p += sfnt_read(p, 8, sfont); /* F2Dot14 x 4 */
	}       
      } while (flags & MORE_COMPONENT);

      /* TrueType instructions */
      if (flags & WE_HAVE_INSTRUCTIONS) {
	USHORT len_inst;
	len_inst = sfnt_get_ushort(sfont);
	p += sfnt_put_ushort(p, len_inst);
	p += sfnt_read(p, len_inst, sfont);
      }

      /* garbage after instructions, glyph data may 4-byte aligned */
      diff = len - (p - save);
      if (diff > 0) {
	p += sfnt_read(p, diff, sfont);
      } else if (diff < 0) { /* maybe it's too late */
	ERROR("TTF file may corrupt");
      }

    } else { /* not composite glyph: just copy */
      p += sfnt_read(p, len - MIN_GLYPH_LEN, sfont);
    }
    currpos += len;

    if (padlen > 0) { /* forcibly aligning */
      memset(p, 0, padlen);
      p += padlen;
      currpos += padlen;
#ifdef DEBUG
      fprintf(stderr, " ** Warning: glyph data (gid %u) ", gid);
      fprintf(stderr, "length not multiple of four **\n");
      fprintf(stderr, " ** Warning: adjusting it (zero filled) **\n");
      fprintf(stderr, " ** Warning: forcibly aligning glyph table **\n");
#endif
    }

  } /* end for */

  glyf_table_size = currpos;

  RELEASE(location);

  /* do not free xxx_data, PUT_TABLE just copy address */
  sfnt_set_table(sfont, "glyf", glyf_table_data, glyf_table_size);

  /* building loca and hmtx table */
  build_other(sfont, g, numHMetrics, currpos);

  maxp->numGlyphs = g->last_gid + 1; /* one for missing char (gid 0) */
  hhea->numberOfHMetrics = g->last_gid + 1;
  head->indexToLocFormat = 1; /* forcing long format */

  /* TODO */
  sfnt_set_table(sfont, "maxp", tt_pack_maxp_table(maxp), TT_MAXP_TABLE_SIZE);
  RELEASE(maxp);
  sfnt_set_table(sfont, "hhea", tt_pack_hhea_table(hhea), TT_HHEA_TABLE_SIZE);
  RELEASE(hhea);
  sfnt_set_table(sfont, "head", tt_pack_head_table(head), TT_HEAD_TABLE_SIZE);
  RELEASE(head);

  return 0;
}
