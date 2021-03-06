/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/tt_table.h,v 1.1 2004/01/08 13:44:15 rahtz Exp $
    
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

#ifndef _TT_TABLE_H_
#define _TT_TABLE_H_

#include "sfnt.h"

#define TT_HEAD_TABLE_SIZE 54UL
#define TT_MAXP_TABLE_SIZE 32UL
#define TT_HHEA_TABLE_SIZE 36UL

struct tt_head_table
{
  Fixed  version;
  Fixed  fontRevision;
  ULONG  checkSumAdjustment;
  ULONG  magicNumber;
  USHORT flags;
  USHORT unitsPerEm;
  BYTE   created[8];
  BYTE   modified[8];
  FWord  xMin, yMin, xMax, yMax;
  USHORT macStyle;
  USHORT lowestRecPPEM;
  SHORT  fontDirectionHint;
  SHORT  indexToLocFormat;
  SHORT  glyphDataFormat;
};

struct tt_hhea_table
{
  Fixed  version;
  FWord  Ascender;
  FWord  Descender;
  FWord  LineGap;
  uFWord advanceWidthMax;
  FWord  minLeftSideBearing;
  FWord  minRightSideBearing;
  FWord  xMaxExtent;
  SHORT  caretSlopeRise;
  SHORT  caretSlopeRun;
  SHORT  reserved[5]; /* set to 0 */
  SHORT  metricDataFormat;
  USHORT numberOfHMetrics;
};

struct tt_maxp_table
{
  Fixed  version;
  USHORT numGlyphs;
  USHORT maxPoints;
  USHORT maxContours;
  USHORT maxComponentPoints;
  USHORT maxComponentContours;
  USHORT maxZones;
  USHORT maxTwilightPoints;
  USHORT maxStorage;
  USHORT maxFunctionDefs;
  USHORT maxInstructionDefs;
  USHORT maxStackElements;
  USHORT maxSizeOfInstructions;
  USHORT maxComponentElements;
  USHORT maxComponentDepth;
};

struct tt_os2__table
{
  USHORT  version; /* 0x0001 or 0x0002 */
  SHORT   xAvgCharWidth;  
  USHORT  usWeightClass;  
  USHORT  usWidthClass;   
  SHORT   fsType;  /* if (faType & 0x08) editable_embedding */
  SHORT   ySubscriptXSize;        
  SHORT   ySubscriptYSize;      
  SHORT   ySubscriptXOffset;
  SHORT   ySubscriptYOffset;      
  SHORT   ySuperscriptXSize;      
  SHORT   ySuperscriptYSize;      
  SHORT   ySuperscriptXOffset;    
  SHORT   ySuperscriptYOffset;    
  SHORT   yStrikeoutSize; 
  SHORT   yStrikeoutPosition;     
  SHORT   sFamilyClass;   
  BYTE    panose[10];
  ULONG   ulUnicodeRange1;
  ULONG   ulUnicodeRange2;
  ULONG   ulUnicodeRange3;
  ULONG   ulUnicodeRange4;
  CHAR    achVendID[4];   
  USHORT  fsSelection;    
  USHORT  usFirstCharIndex;
  USHORT  usLastCharIndex;
  SHORT   sTypoAscender;  /* TTF spec. from MS is wrong */
  SHORT   sTypoDescender; /* TTF spec. from MS is wrong */
  SHORT   sTypoLineGap;   /* TTF spec. from MS is wrong */
  USHORT  usWinAscent;   
  USHORT  usWinDescent;    
  ULONG   ulCodePageRange1;
  ULONG   ulCodePageRange2;
  /* version 0x0002 */
  SHORT   sxHeight;
  SHORT   sCapHeight;
  USHORT  usDefaultChar;
  USHORT  usBreakChar;
  USHORT  usMaxContext;
};

struct tt_post_table
{
  Fixed Format;
  Fixed italicAngle;
  FWord underlinePosition;
  FWord underlineThickness;
  ULONG isFixedPitch;
  ULONG minMemType42;
  ULONG maxMemType42;
  ULONG minMemType1;
  ULONG maxMemType1; 
};

/* glyph names in post table */

extern char   *macglyphorder[];

struct tt_glyph_names
{
  Fixed    version; /* 1.0 or 2.0, 2.5 not supported (deprecated) */
  USHORT   count;   /* number of glyphs */
  USHORT   count2;  /* number of non-standard glyph names */
  char   **glyphs;  /* glyph names */
  char   **names;   /* non-standard glyph names */
};

/* head, hhea, maxp */
extern char  *tt_pack_head_table (struct tt_head_table *table);
extern struct tt_head_table *tt_read_head_table (sfnt *sfont);
extern char  *tt_pack_hhea_table (struct tt_hhea_table *table);
extern struct tt_hhea_table *tt_read_hhea_table (sfnt *sfont);
extern char  *tt_pack_maxp_table (struct tt_maxp_table *table);
extern struct tt_maxp_table *tt_read_maxp_table (sfnt *sfont);

/* post table and PostScript glyph name support */
extern struct tt_post_table *tt_read_post_table (sfnt *sfont);
extern struct tt_glyph_names *tt_get_glyph_names (sfnt *sfont);
extern USHORT tt_glyph_lookup (struct tt_glyph_names *names, char *glyph);
extern void   tt_release_glyph_names (struct tt_glyph_names *names);

/* OS/2 table */
extern struct tt_os2__table *tt_read_os2__table (sfnt *sfont);

/* name table */
extern USHORT tt_get_name (sfnt *sfont, char *dest, USHORT destlen,
			   USHORT plat_id, USHORT enco_id,
			   USHORT lang_id, USHORT name_id);
extern USHORT tt_get_ps_fontname (sfnt *sfont, char *dest, USHORT destlen);

#endif /* _TT_TABLE_H_ */
