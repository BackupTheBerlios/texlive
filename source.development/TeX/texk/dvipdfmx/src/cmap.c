/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/cmap.c,v 1.1 2004/01/08 13:44:13 rahtz Exp $
    
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
#include <string.h>
#include <ctype.h>

#include "system.h"
#include "mem.h"
/* work_buffer */
#include "mfileio.h"
#include "error.h"

#include "ps_parse.h"
#include "cmap.h"

static unsigned char verbose = 0;

void cmap_set_verbose(void)
{
  if (verbose < 255) {
    verbose += 1;
  }
}

#ifdef CMAP_STRICT
static int abort_invalid = 1;
static int abort_missing = 1;
#else
static int abort_invalid = 0;
static int abort_missing = 0;
#endif /* CMAP_STRICT */
static int warn_invalid = 1;
static int warn_missing = 1;

void cmap_set_lookup(int flag)
{
  if (flag & CMAP_NO_INVALID)
    abort_invalid = 1;
  if (flag & CMAP_NO_MISSING)
    abort_missing = 1;
  if (flag & CMAP_NO_WARN_INVALID)
    warn_invalid = 0;
  if (flag & CMAP_NO_WARN_MISSING)
    warn_missing = 0;

  if (verbose) {
    if (flag & CMAP_NO_INVALID) {
      fprintf(stderr, "cmap_set_lookup(): NO_INVALID flag on\n");
    }
    if (flag & CMAP_NO_MISSING) {
      fprintf(stderr, "cmap_set_lookup(): NO_MISSING flag on\n");
    }
  }
}

#define MAX_CMAP 4u
static struct cmap_s *cmaps = NULL;
int max_cmaps = 0, num_cmaps = 0;

#define CMAP_MAX_NESTING 2
static int nest = 0;

/* only double-byte CMaps are supported */
/* codespacerange */
struct cmap_csrange_s
{
  unsigned char hi_start;
  unsigned char hi_end;
  unsigned char lo_start;
  unsigned char lo_end;
};

struct cmap_s
{
  char  *CMapName;
  char  *UseCMap;
  int    use_cmap; /* id */
  int    CMapType;
  double CMapVersion;
  int    WMode;
  struct cid_sysinfo_s *sysinfo;
  int num_csranges;
  struct cmap_csrange_s *csranges;
  unsigned int size;
  unsigned short *offsets;
  unsigned short *map;
};

/* parser */
static int cmap_read (FILE *fp, struct cmap_s *cmap);

/* access CMap info */
char *cmap_cmapname (int id)
{
  if (id >= 0 && id < num_cmaps) {
    return cmaps[id].CMapName;
  } else {
    return NULL;
  }
}

struct cid_sysinfo_s *cmap_sysinfo (int id)
{
  if (id >= 0 && id < num_cmaps) {
    return cmaps[id].sysinfo;
  } else {
    ERROR("Invalid cmap id");
    return NULL;
  }
}

int cmap_wmode (int id)
{
  if (id >= 0 && id < num_cmaps) {
    return cmaps[id].WMode;
  } else {
    ERROR("Invalid cmap id");
    return 0;
  }
}

/*
  check the validity of a input code against codespaceranges.
  returns -1 (invalid) if invalid.
*/
static int check_range (unsigned char hi, unsigned char lo, int id)
{
  int i;
  struct cmap_csrange_s *ranges;

  if (cmaps[id].num_csranges < 1) { /* unknown */
    return 0;
  } else {
    ranges = cmaps[id].csranges;
    for (i=0;i<cmaps[id].num_csranges;i++) {
      if (hi >= ranges[i].hi_start && hi <= ranges[i].hi_end &&
	  lo >= ranges[i].lo_start && lo <= ranges[i].lo_end) {
	return 0;
      }
    }
  }

  return -1; /* invalid */
}

/*
  TODO:
   o Adobe CMap spec. says trying to redefine codespace causes an error
*/

/*
  lookup cmap table
  int id: internally used CMap identifier
  CID (returned value): 16-bit unsgined integer
*/
unsigned short cmap_lookup (unsigned char hi, unsigned char lo, int id)
{
  unsigned short cid = CID_NOTDEF;
  unsigned short offset = 0;

  if (nest > CMAP_MAX_NESTING)
    ERROR("CMap nested too deeply");
  nest++;
  if (id == CMAP_IDENTITY_H || id == CMAP_IDENTITY_V) {
    cid = (hi << 8) + lo;
  } else if (id >= 0 && id < num_cmaps) {
    if (check_range(hi, lo, id) < 0) {
      if (warn_invalid) {
	fprintf(stderr, "\n** Warning: Invalid character 0x%02x%02x **\n",
		hi, lo);
      }
      if (abort_invalid)
	ERROR("cmap_lookup(): invalid character"); /* error */
    } else {
      offset = (cmaps[id].offsets)[hi];
      if (offset == 0xffff) { /* no mapping available */
	cid = CID_NOTDEF;
      } else {
	cid = (cmaps[id].map)[offset+lo];
      }
      if (cid == CID_NOTDEF && cmaps[id].use_cmap >= 0) {
	cid = cmap_lookup(hi, lo, cmaps[id].use_cmap);
      }
    }
  } else {
    fprintf(stderr, "ID: %d/%d\n", id, num_cmaps);
    ERROR("invalid CMap ID");
  }
  nest--;

  if (nest == 0 && cid == CID_NOTDEF) {
    if (warn_missing)
      fprintf(stderr, "\n** Warning: character 0x%02x%02x missing **\n",
	      hi, lo);
    if (abort_missing)
      ERROR("no character available for this code.");
  }


  return cid;
}

static void init_a_cmap(struct cmap_s *cmap)
{
  int i;

  cmap->CMapName = NULL; /* name of CMap */
  cmap->UseCMap = NULL;  /* CMap name that this CMap depend */
  cmap->CMapVersion = 0.0;
  cmap->CMapType = 1;    /* 2 for ToUnicode CMap ? */
  cmap->WMode = 0;       /* writing mode: 0 (horizontal)/1 (vertical) */

  cmap->use_cmap = -1;   /* internal CMap id of `UseCMap' CMap */

  /* CIDSystemInfo */
  cmap->sysinfo = NEW(1, struct cid_sysinfo_s);
  cmap->sysinfo->registry = NULL;
  cmap->sysinfo->ordering = NULL;
  cmap->sysinfo->supplement = 0;

  /* codespaceranges */
  cmap->num_csranges = 0;
  cmap->csranges = NULL;

  /* internal represantation of a character code mapping table */
  cmap->size = 0; /* size of table data */
  cmap->offsets = NEW(256, unsigned short);
  /* offset is at most 255*256 */
  for (i=0; i<256; i++) {
    cmap->offsets[i] = 0xffff; /* no mapping available */
  }
  /* (n x 256) ushorts array */
  cmap->map = NULL;

  return;
}

static void release_a_cmap (int id)
{
  struct cid_sysinfo_s *sysinfo;

  if (id >= 2 && id < num_cmaps) {
    if (cmaps[id].CMapName) RELEASE(cmaps[id].CMapName);
    if (cmaps[id].UseCMap) RELEASE(cmaps[id].UseCMap);
    if ((sysinfo = cmaps[id].sysinfo) != NULL) {
      if (sysinfo->registry) RELEASE(sysinfo->registry);
      if (sysinfo->ordering) RELEASE(sysinfo->ordering);
      RELEASE(sysinfo);
    }
    if (cmaps[id].offsets) RELEASE(cmaps[id].offsets);
    if (cmaps[id].map) RELEASE(cmaps[id].map);
    if (cmaps[id].csranges) RELEASE(cmaps[id].csranges);
  }
}

/* load pre-defined CMaps `Identity-H' and `Identity-V' */
static void init_cmap(void)
{
  max_cmaps += MAX_CMAP;
  cmaps = RENEW(cmaps, max_cmaps, struct cmap_s);
  num_cmaps += 2;

  cmaps[CMAP_IDENTITY_H].CMapName = NEW(11, char);
  strcpy(cmaps[CMAP_IDENTITY_H].CMapName, "Identity-H");
  cmaps[CMAP_IDENTITY_H].UseCMap = NULL;
  cmaps[CMAP_IDENTITY_H].use_cmap = -1;
  cmaps[CMAP_IDENTITY_H].CMapType = 1;
  cmaps[CMAP_IDENTITY_H].CMapVersion = 10.001;
  cmaps[CMAP_IDENTITY_H].WMode = 0;
  cmaps[CMAP_IDENTITY_H].sysinfo = NEW(1, struct cid_sysinfo_s);
  (cmaps[CMAP_IDENTITY_H].sysinfo)->registry = NEW(6, char);
  (cmaps[CMAP_IDENTITY_H].sysinfo)->ordering = NEW(9, char);
  strcpy((cmaps[CMAP_IDENTITY_H].sysinfo)->registry, "Adobe");
  strcpy((cmaps[CMAP_IDENTITY_H].sysinfo)->ordering, "Identity");
  (cmaps[CMAP_IDENTITY_H].sysinfo)->supplement = 0;
  cmaps[CMAP_IDENTITY_H].num_csranges = 0;
  cmaps[CMAP_IDENTITY_H].csranges = NULL;
  cmaps[CMAP_IDENTITY_H].size = 0;
  cmaps[CMAP_IDENTITY_H].offsets = NULL;
  cmaps[CMAP_IDENTITY_H].map = NULL;


  cmaps[CMAP_IDENTITY_V].CMapName = NEW(11, char);
  strcpy(cmaps[CMAP_IDENTITY_V].CMapName, "Identity-V");
  cmaps[CMAP_IDENTITY_V].UseCMap = NULL;
  cmaps[CMAP_IDENTITY_V].use_cmap = -1;
  cmaps[CMAP_IDENTITY_V].CMapType = 1;
  cmaps[CMAP_IDENTITY_V].CMapVersion = 10.001;
  cmaps[CMAP_IDENTITY_V].WMode = 1;
  cmaps[CMAP_IDENTITY_V].sysinfo = NEW(1, struct cid_sysinfo_s);
  (cmaps[CMAP_IDENTITY_V].sysinfo)->registry = NEW(6, char);
  (cmaps[CMAP_IDENTITY_V].sysinfo)->ordering = NEW(9, char);
  strcpy((cmaps[CMAP_IDENTITY_V].sysinfo)->registry, "Adobe");
  strcpy((cmaps[CMAP_IDENTITY_V].sysinfo)->ordering, "Identity");
  (cmaps[CMAP_IDENTITY_V].sysinfo)->supplement = 0;
  cmaps[CMAP_IDENTITY_V].num_csranges = 0;
  cmaps[CMAP_IDENTITY_V].csranges = NULL;
  cmaps[CMAP_IDENTITY_V].size = 0;
  cmaps[CMAP_IDENTITY_V].offsets = NULL;
  cmaps[CMAP_IDENTITY_V].map = NULL;

  return;
}

/* called recursively */
static int load_cmap (const char *cmap_name)
{
  int cmap_id = 0;
  char *full_filename = NULL;
  FILE *fp;

#ifndef MIKTEX 
  char *cmap_name_ext; 
  int len = strlen(cmap_name); 
 
  if (len > 5 && strncmp(cmap_name+len-5, ".cmap", 5) == 0) { 
    cmap_name_ext = NEW (len-4, char); 
    strncpy(cmap_name_ext, cmap_name, len-5); 
    *(cmap_name_ext+len-4) = 0; 
  } else { 
    cmap_name_ext = NEW (len+6, char); 
    strcpy(cmap_name_ext, cmap_name); 
    strcat(cmap_name_ext, ".cmap"); 
  } 
#endif 

#ifdef MIKTEX
  if (!miktex_find_app_input_file("dvipdfm", cmap_name, full_filename = work_buffer)) {
    if (miktex_get_acrobat_font_dir(work_buffer)) {
      strcat(work_buffer, "\\..\\CMap\\");
      strcat(work_buffer, cmap_name);
      if (fp = MFOPEN (work_buffer, FOPEN_RBIN_MODE)) goto cmap_found;
    }
    full_filename = NULL;
  }
#else
  if ((full_filename = kpse_find_file (cmap_name,
				       kpse_program_text_format,
				       0)) == NULL &&
      (full_filename = kpse_find_file (cmap_name_ext,
				       kpse_program_text_format,
				       0)) == NULL) {
    kpse_reset_program_name("cmap");
    if ((full_filename = kpse_find_file(cmap_name,
				        kpse_program_text_format,
					0)) == NULL)
      full_filename = kpse_find_file(cmap_name_ext,
		      		     kpse_program_text_format,
				     0);
    kpse_reset_program_name("dvipdfm");
  }
  RELEASE (cmap_name_ext);
#endif
  /* don't stop here ! someone just test existence of CMap */
  if (full_filename == NULL ||
      (fp = MFOPEN (full_filename, FOPEN_RBIN_MODE)) == NULL)
    return -1;

cmap_found:
  if (verbose)
    fprintf(stderr, "(CMap:%s", cmap_name);

  if (verbose > 1)
    fprintf(stderr, "(%s)", full_filename);

  if (num_cmaps >= max_cmaps) {
    max_cmaps += MAX_CMAP;
    cmaps = RENEW(cmaps, max_cmaps, struct cmap_s);
  }
  cmap_id = num_cmaps;
  num_cmaps += 1; /* here */

  init_a_cmap(cmaps+cmap_id);
  if (cmap_read(fp, cmaps+cmap_id) < 0) {
    ERROR("reading CMap file failed");
  }

  MFCLOSE(fp);

  if (cmaps[cmap_id].sysinfo->registry == NULL ||
      cmaps[cmap_id].sysinfo->ordering == NULL) {
    ERROR("character collection not determined");
  }

  if (cmaps[cmap_id].UseCMap) {
    if (verbose)
      fprintf(stderr, "(usecmap:%s)", cmaps[cmap_id].UseCMap);
    cmaps[cmap_id].use_cmap = get_cmap(cmaps[cmap_id].UseCMap);
  } else {
    cmaps[cmap_id].use_cmap = -1;
  }

  if (verbose)
    fprintf(stderr, ")");

  return cmap_id;
}

/* load CMap `cmap_name', returns CMap id */
int get_cmap (const char *cmap_name)
{
  int result = -1;
  int i;

  if (num_cmaps == 0) init_cmap();

  for (i=0;i<num_cmaps;i++) {
    if (cmap_name && (strcmp(cmap_name, cmaps[i].CMapName) == 0)) {
	result = i;
	break;
    }
  }
  if (result < 0) {
    result = load_cmap(cmap_name);
    /* num_cmaps++ should not come here ! */
  }

  return result;
}

void cmap_close_all (void)
{
  int i;

  if (cmaps) {
    for (i=0;i<num_cmaps;i++) {
      release_a_cmap(i);
    }
    RELEASE(cmaps);
  }
}

/*
  Here is a simple CMap resource parser written in C. (not complete)

  It parses CMap resource file, and stores content of CMap file as
  a structure cmap_s (see cmap.h). The character mapping information, which
  appears in between begin(bf|cid)(range|char) and end(bf|cid)(char|range),
  is decoded and is converted to a 256 offsets array and an array of
  destination CIDs (or codes for to-code mapping file). All input code (and
  output code) is restricted to (up to) 16-bit long. The destination CID array
  is segmented into 256 uint16 sequences according as their input code's first
  byte value. The offsets array maps first byte of a 2-byte sequence to an
  offset of corresponding segment in the destination CID (code) array. If
  offset is equals to 0xffff, then no mapping is available and the input code
  should be mapped to .notdef. (when input code is not invalid)

  It works, at least, for most Adobe-Japan1-4 Adobe CMaps.

  Limitations:

   o CIDSystemInfo must be in single (and simple) dict form:
      /CIDSystemInfo n dict dup begin ... end def

   o only accept `value /Key exch def' and `/Key value def' form.
     everything should be written in simple form.
     Dictionaries in the << /Key value ... >> form causes error.

   o stack operation such as dup, index, roll and exch in general are
     not supported.

   o string must be in (...) form, ascii hex and ascii base 85 are not
     supported (decode filter is supplied in filter.c, though)

   o srcCode and dstCode are limited to double-byte sequences.

   o in bfchar and bfrange, charNames are ignored.

   o XUID and UIDOffset are ignored.

   o notdefrange and notdefchar are ignored.

   o rearrangedfont will not be supported.
*/

/* CMap parser status */
#define CONTINUE    PS_PARSE_OK
#define FINISH      (PS_PARSE_STATUS_MAX + 1)
#define PARSE_ERROR (PS_PARSE_ERROR_MIN - 1)
#define STACK_ERROR (PS_PARSE_ERROR_MIN - 2)
#define TYPE_ERROR  (PS_PARSE_ERROR_MIN - 3)
#define MEM_ERROR   (PS_PARSE_ERROR_MIN - 4)
#define RANGE_ERROR (PS_PARSE_ERROR_MIN - 5)

static int token_type = PS_UNKNOWN;

#define MATCH(s1, s2) (s1 && s2 && strncmp(s1, s2, WORK_BUFFER_SIZE) == 0)

/*
  operand stack
   - for usecmap, begin(bf|cid)(range|char), and /Key exch def.
*/

#define MAX_STACK_DEPTH 32
static int stack_top = 0;
static struct {
  int   type;
  char *data;
} stack[MAX_STACK_DEPTH];

/* operand stack operation */
static void push (int type, char *data, int *status)
{
  if (stack_top < MAX_STACK_DEPTH) {
    stack[stack_top].type = type;
    stack[stack_top].data = NEW(strlen(data)+1, char);
    strcpy(stack[stack_top].data, data);
    stack_top++;
  } else {
    fprintf(stderr, "stack overflow\n");
    *status = STACK_ERROR;
  }
}

static void pop (int *status)
{
  if (stack_top < 1) {
    fprintf(stderr, "stack underflow\n");
    *status = STACK_ERROR;
  } else {
    if (stack[stack_top-1].data)
      RELEASE(stack[stack_top-1].data);
    stack_top--;
  }
}

static void clear (int *status)
{
  while (stack_top > 0) {
    stack_top--;
    if (stack[stack_top].data)
      RELEASE(stack[stack_top].data);
  }
}

/*
  get xxx from stack or file

   o from_stack forces values to be taken from stack.

   o When from_stack is equals to 0 and the next token is `exch', values
     are taken from stack and pop() is invoked.
*/
/* TODO: hex string and ascii85 encoded string */
static char *get_name_string_value (FILE *fp, int *status, int from_stack,
				    int type)
{
  char *result = NULL;

  if (type & (PS_STRING_AHX | PS_STRING_A85)) {
    fprintf(stderr, "ascii hex/ascii85-encoded string type not supported\n");
    return NULL;
  }

  if (!from_stack) {
    *status = ps_parse_get_token(fp, &token_type,
				 work_buffer, WORK_BUFFER_SIZE);
    if (*status == PS_PARSE_OK) {
      if (token_type == type) {
	result = NEW(strlen(work_buffer)+1, char);
	strcpy(result, work_buffer);
      } else if (token_type == PS_TOKEN && MATCH(work_buffer, "exch")) {
	from_stack = 1;
      } else {
	*status = TYPE_ERROR;
      }
    }
  }

  if (from_stack) {
    if (stack_top > 0) {
      if (stack[stack_top-1].type == type) {
	result = NEW(strlen(stack[stack_top-1].data)+1, char);
	strcpy(result, stack[stack_top-1].data);
      } else {
	*status = TYPE_ERROR;
      }
      pop(status);
    } else {
      fprintf(stderr, "stack underflow\n");
      *status = STACK_ERROR;
    }
  }

  return result;
}

#define get_name_value(f,s,l) get_name_string_value(f, s, l, PS_NAME)
#define get_string_value(f,s,l) get_name_string_value(f, s, l, PS_STRING)

/* TODO: radix */
static int get_integer_value (FILE *fp, int *status, int from_stack)
{
  int result = 0;

  if (!from_stack) {
    *status = ps_parse_get_token(fp, &token_type,
				 work_buffer, WORK_BUFFER_SIZE);
    if (*status == PS_PARSE_OK) {
      if (token_type == PS_INTEGER) {
	result = atoi(work_buffer);
      } else if (token_type == PS_TOKEN && MATCH(work_buffer, "exch")) {
	from_stack = 1;
      } else {
	*status = TYPE_ERROR;
      }
    }
  }

  if (from_stack) {
    if (stack_top > 0) {
      if (stack[stack_top-1].type == PS_INTEGER) {
	result = atoi(stack[stack_top-1].data);
      } else {
	*status = TYPE_ERROR;
      }
      pop(status);
    } else {
      fprintf(stderr, "stack underflow\n");
      *status = STACK_ERROR;
    }
  }

  return result;
}

static double get_real_value (FILE *fp, int *status, int from_stack)
{
  double result = 0.0;

  if (!from_stack) {
    *status = ps_parse_get_token(fp, &token_type,
				 work_buffer, WORK_BUFFER_SIZE);
    if (*status == PS_PARSE_OK) {
      if (token_type == PS_INTEGER || token_type == PS_REAL) {
	result = atof(work_buffer);
      } else if (token_type == PS_TOKEN && MATCH(work_buffer, "exch")) {
	from_stack = 1;
      } else {
	*status = TYPE_ERROR;
      }
    }
  }

  if (from_stack) {
    if (stack_top > 0) {
      if (stack[stack_top-1].type == PS_INTEGER ||
	  stack[stack_top-1].type == PS_REAL) {
	result = atof(stack[stack_top-1].data);
      } else {
	*status = TYPE_ERROR;
      }
      pop(status);
    } else {
      fprintf(stderr, "stack underflow\n");
      *status = STACK_ERROR;
    }
  }
  
  return result;
}

/*
  skip an array

  o each elements of array is restricted to types specified in accept_type.
    e.g.: skip_array(fp, &status, PS_INTEGER|PS_REAL)

  o nested arrays are not supported
*/
static int skip_array (FILE *fp, int *status, int accept_type)
{
  if (verbose > 4)
    fprintf(stderr, "skipping array\n");

  while ((*status = ps_parse_get_token(fp, &token_type,
				       work_buffer, WORK_BUFFER_SIZE))
	 == PS_PARSE_OK) {
    if (token_type == PS_END_ARRAY) {
      break;
    } else if (!(token_type & accept_type)) {
      *status = TYPE_ERROR;
      return -1;
    }
  } /* while */

  return 0;
}

/* next token must be `token' */
static int expect_token (FILE *fp, const char *token)
{
  int status;

  status = ps_parse_get_token(fp, &token_type, work_buffer, WORK_BUFFER_SIZE);
  if (status != PS_PARSE_OK) {
    return status;
  } else if (token_type == PS_TOKEN && MATCH(work_buffer, token)) {
    return CONTINUE;
  } else {
    if (verbose > 1) {
      fprintf(stderr, "\nexpecting `%s:'\n", token);
      fprintf(stderr, "=== current input buffer ===\n");
      fprintf(stderr, "%s\n", work_buffer);
      fprintf(stderr, "============================\n");
    }
    return PARSE_ERROR;
  }
}

/* build mapping table */
static int add_maprange (struct cmap_s *t, unsigned short srclo,
			 unsigned short srchi, unsigned short dest)
{
  unsigned char idx;
  unsigned short offset;
  unsigned char lo, hi;
  int i;

  idx = (srclo >> 8) & 0xff;
  if (idx == ((srchi >> 8) & 0xff)) { /* not sure - see, technote #5014 */
    offset = (t->offsets)[idx];
    if (offset == 0xffff) {
      (t->offsets)[idx] = offset = t->size;
      t->size += 256;
      t->map = RENEW(t->map, t->size, unsigned short);
      for (i=offset; i < offset+256; i++) {
	(t->map)[i] = CID_NOTDEF;
      }
    }
    lo = (srclo & 0xff);
    hi = (srchi & 0xff);
    for (i = offset+lo; i <= offset+hi; i++) {
      (t->map)[i] = dest++;
    }
    return 0;
  } else { /* invalid */
    fprintf(stderr, "first bytes of srclo and srchi mismatched.\n");
    return -1;
  }
}

/*
  parse <destCodeLo> <destCodeHi>

  returns -1 if destCodeLo and/or destCodeHi > 0xffff
*/
static int get_range (FILE *fp, int *status,
		      unsigned short *lo, unsigned short *hi)
{
  int accept = 0;
  unsigned long value;

  /* srcCode must be in hex string notation */
  *status = ps_parse_get_token(fp, &token_type, work_buffer, WORK_BUFFER_SIZE);
  if (*status == PS_PARSE_OK && token_type == PS_STRING_AHX) {
    if ((value = strtoul(work_buffer, (char **) NULL, 16)) > CID_MAX_CODE) {
      accept = -1;
    } else {
      *lo = (unsigned short)value;
    }
  } else {
    fprintf(stderr, "unexpected token");
    *status = PARSE_ERROR;
    return -1;
  }
  *status = ps_parse_get_token(fp, &token_type, work_buffer, WORK_BUFFER_SIZE);
  if (*status == PS_PARSE_OK && token_type == PS_STRING_AHX) {
    if ((value = strtoul(work_buffer, (char **) NULL, 16)) > CID_MAX_CODE) {
      accept = -1;
    } else {
      *hi = (unsigned short)value;
    }
  } else {
    fprintf(stderr, "unexpected token");
    *status = PARSE_ERROR;
    return -1;
  }

  return accept;
}

static int get_codespacerange (FILE *fp, int *status,
			       int len, struct cmap_s *t)
{
  int i = 0;
  int accept = 0;
  unsigned short start, end;

  if (verbose > 4) 
    fprintf(stderr, "reading codespacerange (%d)\n", len);

  t->num_csranges = len;
  t->csranges = NEW(len, struct cmap_csrange_s);

  while (len-- > 0) {
    accept = get_range(fp, status, &start, &end);
    if (*status == CONTINUE) {
      if (accept < 0) {
	if (verbose > 1)
	  fprintf(stderr, "ignoring multibyte char > 65535\n");
	t->num_csranges--;
      } else {
	(t->csranges)[i].lo_start = start & 0xff;
	(t->csranges)[i].lo_end = end & 0xff;
	(t->csranges)[i].hi_start = (start >> 8) & 0xff;
	(t->csranges)[i].hi_end = (end >> 8) & 0xff;
	i++;
      }
    } else {
      RELEASE(t->csranges);
      t->num_csranges = 0;
      t->csranges = NULL;
      return -1;
    }
  }

  if ((*status = expect_token(fp, "endcodespacerange")) != CONTINUE) {
    RELEASE(t->csranges);
    t->num_csranges = 0;
    t->csranges = NULL;
    return -1;
  }

  return 0;
}

/* ignore this */
static int get_notdefrange (FILE *fp, int *status, int len)
{
  int accept = 0;
  unsigned short start, end;

  if (verbose > 4) 
    fprintf(stderr, "reading notdefrange (%d)\n", len);

  while (len-- > 0) {
    accept = get_range(fp, status, &start, &end);
    if (*status == CONTINUE) {
      *status = ps_parse_get_token(fp, &token_type,
				   work_buffer, WORK_BUFFER_SIZE);
      if (token_type == PS_INTEGER) { /* destCID: integer */
	/* do something */
      } else {
	*status = TYPE_ERROR;
	return -1;
      }
    } else {
      return -1;
    }
  }

  if ((*status = expect_token(fp, "endnotdefrange")) != CONTINUE)
    return -1;

  return 0;
}

static int get_notdefchar (FILE *fp, int *status, int len)
{
  unsigned long start, end;

  if (verbose > 4) 
    fprintf(stderr, "reading cidchar (%d)\n", len);

  while (len-- > 0) {
    *status = ps_parse_get_token(fp, &token_type,
				 work_buffer, WORK_BUFFER_SIZE);
    if (*status == PS_PARSE_OK) {
      if (token_type == PS_STRING_AHX) { /* srcCode must be in hex notation */
	start = strtoul(work_buffer, (char **) NULL, 16);
      } else {
	*status = TYPE_ERROR;
	return -1;
      }
    } else {
      return -1;
    }
    *status = ps_parse_get_token(fp, &token_type,
				 work_buffer, WORK_BUFFER_SIZE);
    if (*status == PS_PARSE_OK) {
      if (token_type == PS_INTEGER) { /* destCID */
	if ((end = strtoul(work_buffer, (char **) NULL, 10)) > CID_MAX_CID) {
	  if (verbose)
	    fprintf(stderr, "found CID > 65535 !\n");
	  *status = RANGE_ERROR;
	  return -1;
	}
      } else {
	*status = TYPE_ERROR;
	return -1;
      }
    } else {
      return -1;
    }
    if (start > CID_MAX_CODE) { /* ignore */
      if (verbose > 1)
	fprintf(stderr, "ignoring multibyte char > 65535\n");
    } else {
      /* do something */
    }
  } /* while */

  /* do something */
  if ((*status = expect_token(fp, "endbfchar")) != CONTINUE)
    return -1;

  return -1;
}

/* destCID must be integer (not hex string) */
static int get_cidrange (FILE *fp, int *status, int len, struct cmap_s *t)
{
  int accept = 0;
  unsigned short srclo, srchi;
  unsigned long destlo;

  if (verbose > 4) 
    fprintf(stderr, "reading cidrange (%d)\n", len);

  while (len-- > 0) {
    accept = get_range(fp, status, &srclo, &srchi);
    if (*status == CONTINUE) {
      *status = ps_parse_get_token(fp, &token_type,
				   work_buffer, WORK_BUFFER_SIZE);
      if (*status == PS_PARSE_OK) {
	if (token_type == PS_INTEGER) {
	  if (accept < 0 ||
	      (destlo = strtoul(work_buffer, (char **) NULL, 10)) > CID_MAX_CID) {
	    if (verbose > 1)
	      fprintf(stderr, "ignoring multibyte char > 65535\n");
	  } else {
	    add_maprange(t, srclo, srchi, (unsigned short)destlo);
	  }
	} else {
	  *status = TYPE_ERROR;
	  return -1;
	}
      }
    } else { /* parse error */
      return -1;
    }
  } /* while */

  if ((*status = expect_token(fp, "endcidrange")) != CONTINUE)
    return -1;

  return 0;
}

static int get_cidchar (FILE *fp, int *status, int len, struct cmap_s *t)
{
  unsigned long src, dest;

  if (verbose > 4) 
    fprintf(stderr, "reading cidchar (%d)\n", len);

  while (len-- > 0) {
    *status = ps_parse_get_token(fp, &token_type,
				 work_buffer, WORK_BUFFER_SIZE);
    if (*status == PS_PARSE_OK) {
      if (token_type == PS_STRING_AHX) { /* src must be hex string (?) */
	src = strtoul(work_buffer, (char **) NULL, 16);
      } else {
	*status = TYPE_ERROR;
	return -1;
      }
    } else {
      return -1;
    }
    *status = ps_parse_get_token(fp, &token_type,
				 work_buffer, WORK_BUFFER_SIZE);
    if (*status == PS_PARSE_OK) {
      if (token_type == PS_INTEGER) {
	dest = strtoul(work_buffer, (char **) NULL, 10);
	if (dest > CID_MAX_CID) { /* error */
	  if (verbose)
	    fprintf(stderr, "found CID > 65535 !\n");
	  *status = RANGE_ERROR;
	  return -1;
	}
      } else {
	*status = TYPE_ERROR;
	return -1;
      }
    } else {
      return -1;
    }
    if (src > CID_MAX_CODE) { /* ignore */
      if (verbose > 4)
	fprintf(stderr, "ignoring multibyte char > 65535\n");
    } else {
      add_maprange(t, (unsigned short)src, (unsigned short)src, (unsigned short)dest);
    }
  } /* while */

  if ((*status = expect_token(fp, "endcidchar")) != CONTINUE)
    return -1;

  return 0;
}

/* charName is ignored */
static int get_bfrange (FILE *fp, int *status, int len, struct cmap_s *t)
{
  int accept = 0;
  unsigned short srclo, srchi;
  unsigned long destlo = 0UL;

  if (verbose > 4)
    fprintf(stderr, "reading bfrange (%d)\n", len);

  while (len-- > 0) {
    accept = get_range(fp, status, &srclo, &srchi);
    if (*status != PS_PARSE_OK)
      return -1;
    *status = ps_parse_get_token(fp, &token_type,
				 work_buffer, WORK_BUFFER_SIZE);
    if (*status != PS_PARSE_OK)
      return -1;
    if (token_type == PS_INTEGER) {
      destlo = strtoul(work_buffer, (char **) NULL, 10);
    } else if (token_type == PS_STRING_AHX) {
      destlo = strtoul(work_buffer, (char **) NULL, 16);
    } else if (token_type == PS_BEGIN_ARRAY) {
      accept = -1;
      skip_array(fp, status, PS_NAME);
    } else {
      *status = TYPE_ERROR;
      return -1;
    }
    if (accept < 0 || destlo > CID_MAX_CODE) { /* not supported, just ignore */
      if (verbose > 1)
	fprintf(stderr, "ignoring multibyte char > 65535 or CharNames\n");
    } else {
      add_maprange(t, srclo, srchi, (unsigned short)destlo);
    }
  }

  if ((*status = expect_token(fp, "endbfrange")) != CONTINUE)
    return -1;

  return 0;
}

static int get_bfchar (FILE *fp, int *status, int len, struct cmap_s *t)
{
  unsigned long src, dest;

  if (verbose > 4) 
    fprintf(stderr, "reading cidchar (%d)\n", len);

  while (len-- > 0) {
    *status = ps_parse_get_token(fp, &token_type,
				 work_buffer, WORK_BUFFER_SIZE);
    if (*status == PS_PARSE_OK) {
      if (token_type == PS_STRING_AHX) { /* srcCode must be in hex notation */
	src = strtoul(work_buffer, (char **) NULL, 16);
      } else {
	*status = TYPE_ERROR;
	return -1;
      }
    } else {
      return -1;
    }
    *status = ps_parse_get_token(fp, &token_type,
				 work_buffer, WORK_BUFFER_SIZE);
    if (*status == PS_PARSE_OK) {
      if (token_type == PS_STRING_AHX) {
	dest = strtoul(work_buffer, (char **) NULL, 16);
      } else if (token_type == PS_NAME) {
	dest = CID_MAX_CODE + 1; /* ignore it */
      } else {
	*status = TYPE_ERROR;
	return -1;
      }
    } else {
      return -1;
    }
    if (src > CID_MAX_CODE || dest > CID_MAX_CODE) { /* ignore */
      if (verbose > 1)
	fprintf(stderr, "ignoring multibyte char > 65535 or CharName\n");
    } else {
      add_maprange(t, (unsigned short)src, (unsigned short)src, (unsigned short)dest);
    }
  } /* while */

  /* do something */
  if ((*status = expect_token(fp, "endbfchar")) != CONTINUE)
    return -1;

  return -1;
}

static int cmap_read (FILE *fp, struct cmap_s *cmap)
{
  int status = PARSE_ERROR;
  int len = 0;
#define TOPLEVEL 0
#define SYSINFO 1
  int phase = TOPLEVEL;

  /* check initialization */
  if (cmap == NULL || cmap->sysinfo == NULL) {
    fprintf(stderr, "struct cmap not properly initialized\n");
    return -1;
  }

  /* check first comment line of file */
  if (ps_parse_read_line(fp, work_buffer, WORK_BUFFER_SIZE) == PS_PARSE_OK) {
    if (strncmp(work_buffer, "%!PS", 4) ||
	strstr(work_buffer, "Resource-CMap") == NULL) {
      fprintf(stderr, "not a CMap resource file ?\n");
      return -1;
    }
  } else {
    fprintf(stderr, "parsing file failed\n");
    return -1;
  }

  /* clear stack */
  clear(&status);

  /* skip until begincmap */
  while ((status = ps_parse_get_token(fp, &token_type,
				      work_buffer, WORK_BUFFER_SIZE))
	 == PS_PARSE_OK) {
    if (token_type == PS_TOKEN && MATCH(work_buffer, "begincmap")) {
      break;
    } else if (token_type == PS_TOKEN &&
	       MATCH(work_buffer, "beginrearrangedfont")) {
      fprintf(stderr, "rearrangedfont not supported\n");
      return -1;
    }
  }

  if (status != PS_PARSE_OK) { /* begincmap not found */
    fprintf(stderr, "begincmap not found\n");
    fprintf(stderr, "parsing file failed\n");
    return -1;
  } /* else continue */

  while (status == CONTINUE &&
	 (status = ps_parse_get_token(fp, &token_type,
				      work_buffer, WORK_BUFFER_SIZE))
	 == PS_PARSE_OK) {
    if (phase == TOPLEVEL && token_type == PS_NAME) {
      if (MATCH(work_buffer, "CMapName")) {
	cmap->CMapName = get_name_value(fp, &status, 0);
	if (status == CONTINUE) {
	  if (expect_token(fp, "def") != CONTINUE)
	    status = PARSE_ERROR;
	}
      } else if (MATCH(work_buffer, "WMode")) {
	cmap->WMode = get_integer_value(fp, &status, 0);
	if (status == CONTINUE) {
	  if (expect_token(fp, "def") != CONTINUE)
	    status = PARSE_ERROR;
	}
      } else if (MATCH(work_buffer, "CMapType")) {
	cmap->CMapType = get_integer_value(fp, &status, 0);
	if (status == CONTINUE) {
	  if (expect_token(fp, "def") != CONTINUE)
	    status = PARSE_ERROR;
	}
      } else if (MATCH(work_buffer, "CMapVersion")) {
	cmap->CMapVersion = get_real_value(fp, &status, 0);
	if (status == CONTINUE) {
	  if (expect_token(fp, "def") != CONTINUE)
	    status = PARSE_ERROR;
	}
      } else if (MATCH(work_buffer, "CIDSystemInfo")) {
	/* 
	   It works only for
	    /CIDSystemInfo 3 dict dup begin ... end def
	*/
	status = ps_parse_get_token(fp, &token_type,
				    work_buffer, WORK_BUFFER_SIZE);
	if (token_type == PS_INTEGER) { /* does << ... >> allowed ? */
	  len = atoi(work_buffer);
	  if (expect_token(fp, "dict") != CONTINUE ||
	      expect_token(fp, "dup") != CONTINUE ||
	      expect_token(fp, "begin") != CONTINUE) { /* wrong */
	    status = PARSE_ERROR;
	  }
	} else {
	  status = PARSE_ERROR;
	}
	phase = SYSINFO;
      } else if (MATCH(work_buffer, "XUID")) {
	/* ignore an array of integers */
	status = ps_parse_get_token(fp, &token_type,
				    work_buffer, WORK_BUFFER_SIZE);
	if (token_type == PS_BEGIN_ARRAY) { /* array of integers */
	  skip_array(fp, &status, PS_INTEGER);
	} else {
	  status = PARSE_ERROR;
	}
      } else if (MATCH(work_buffer, "UIDOffset")) {
	get_integer_value(fp, &status, 0); /* ignore */
	if (status == CONTINUE) {
	  if (expect_token(fp, "def") != CONTINUE) {
	    status = PARSE_ERROR;
	  }
	}
      } else {
	push(token_type, work_buffer, &status); /* push name */
      } /* end if TOPLEVEL && PS_NAME */
    } else if (phase == SYSINFO && token_type == PS_NAME) {
      if (MATCH(work_buffer, "Registry")) {
	cmap->sysinfo->registry = get_string_value(fp, &status, 0);
	if (status == CONTINUE) {
	  if (expect_token(fp, "def") != CONTINUE)
	    status = PARSE_ERROR;
	}
      } else if (MATCH(work_buffer, "Ordering")) {
	cmap->sysinfo->ordering = get_string_value(fp, &status, 0);
	if (status == CONTINUE) {
	  if (expect_token(fp, "def") != CONTINUE)
	    status = PARSE_ERROR;
	}
      } else if (MATCH(work_buffer, "Supplement")) {
	cmap->sysinfo->supplement = get_integer_value(fp, &status, 0);
	if (status == CONTINUE) {
	  if (expect_token(fp, "def") != CONTINUE)
	    status = PARSE_ERROR;
	}
      } else {
	if (verbose)
	  fprintf(stderr, "unexpected token in parsing CIDSystemInfo\n");
	status = PARSE_ERROR;
      } /* end if SYSINFO && PS_NAME */
    } else if (phase == SYSINFO && token_type == PS_TOKEN) {
      if (MATCH(work_buffer, "end") && expect_token(fp, "def") == CONTINUE) {
	phase = TOPLEVEL;
      } else {
	status = PARSE_ERROR;
      } /* end if SYSINFO && PS_TOKEN */
    } else if (phase == TOPLEVEL && token_type == PS_TOKEN) {
      if (verbose > 4)
	fprintf(stderr, "token: %s\n", work_buffer);
      if (MATCH(work_buffer, "endcmap")) { /* endcmap found */
	status = FINISH;
      } else if (MATCH(work_buffer, "usecmap")) {
	if (cmap->UseCMap) {
	  fprintf(stderr, "\n*** multiple invokation of usecmap ***\n");
	}
	cmap->UseCMap = get_name_value(fp, &status, 1);
/*
  CMap spec. says length of *range and *char should not exceed 100.
  However, some CMaps does not follow this recommendation.
*/
#define MAX_LEN 256
      } else if (MATCH(work_buffer, "begincodespacerange")) {
	if ((len = get_integer_value(fp, &status, 1)) > MAX_LEN)
	  status = RANGE_ERROR;
	if (status == CONTINUE) {
	  get_codespacerange(fp, &status, len, cmap);
	}
      } else if (MATCH(work_buffer, "beginnotdefrange")) { /* TODO */
	if ((len = get_integer_value(fp, &status, 1)) > MAX_LEN)
	  status = RANGE_ERROR;
	if (status == CONTINUE) {
	  get_notdefrange(fp, &status, len);
	}
      } else if (MATCH(work_buffer, "beginnotdefchar")) { /* TODO */
	if ((len = get_integer_value(fp, &status, 1)) > MAX_LEN)
	  status = RANGE_ERROR;
	if (status == CONTINUE) {
	  get_notdefchar(fp, &status, len);
	}
      } else if (MATCH(work_buffer, "begincidrange")) {
	if ((len = get_integer_value(fp, &status, 1)) > MAX_LEN)
	  status = RANGE_ERROR;
	if (status == CONTINUE) {
	  get_cidrange(fp, &status, len, cmap);
	}
      } else if (MATCH(work_buffer, "begincidchar")) {
	if ((len = get_integer_value(fp, &status, 1)) > MAX_LEN)
	  status = RANGE_ERROR;
	if (status == CONTINUE) {
	  get_cidchar(fp, &status, len, cmap);
	}
      } else if (MATCH(work_buffer, "beginbfrange")) {
	if ((len = get_integer_value(fp, &status, 1)) > MAX_LEN)
	  status = RANGE_ERROR;
	if (status == CONTINUE) {
	  get_bfrange(fp, &status, len, cmap);
	}
      } else if (MATCH(work_buffer, "beginbfchar")) {
	if ((len = get_integer_value(fp, &status, 1)) > MAX_LEN)
	  status = RANGE_ERROR;
	if (status == CONTINUE) {
	  get_bfchar(fp, &status, len, cmap);
	}
      } else if (MATCH(work_buffer, "usefont")) {
	fprintf(stderr, "`usefont' appeared in CMap file\n");
	pop(&status);
      } else {
	clear(&status); /* don't know how to treat this operator */
      } /* end if TOPLEVEL && PS_TOKEN */
    } else if (token_type == PS_STRING ||
	       token_type == PS_STRING_AHX || token_type == PS_STRING_A85 ||
	       token_type == PS_INTEGER || token_type == PS_RADIX ||
	       token_type == PS_REAL) {
      push(token_type, work_buffer, &status);
    } else {
      fprintf(stderr, "parsing file failed\n");
      status = PARSE_ERROR;
    } /* end if */
  } /* while */

  if (verbose > 4 && stack_top > 0)
    fprintf(stderr, "stack not empty: %d\n", stack_top);

  clear(&status);

  if (status != FINISH) {
    fprintf(stderr, "parsing file failed\n");
    return -1;
  }

  return 0;
}
