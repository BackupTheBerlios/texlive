/*  $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/texlive/Repository/source.development/TeX/texk/dvipdfmx/src/cid_basefont.h,v 1.1 2004/01/08 13:44:13 rahtz Exp $
    
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

#ifndef _CID_BASEFONT_H_
#define _CID_BASEFONT_H_

#include "cid.h"

/* PDF object xxx is treated as yyy in this file */
#define NAME       char *
#define INT        double
#define ARRAY      char *
#define STRING     char *
#define DICTIONARY char *

/* Flags */
#define FIXEDWIDTH (1 << 0)  /* Fixed-width font */
#define SERIF      (1 << 1)  /* Serif font */
#define SYMBOLIC   (1 << 2)  /* Symbolic font */
#define SCRIPT     (1 << 3)  /* Script font */
#define STANDARD   (1 << 5)  /* Uses Adobe Standard Roman Character Set */
#define ITALIC     (1 << 6)  /* Italic */
#define ALLCAP     (1 << 16) /* All-cap font */
#define SMALLCAP   (1 << 17) /* Small-cap font */
#define FOCEBOLD   (1 << 18) /* Force bold at small text sizes */

/*
  CID font embedding is not supported, so related stuff are omitted.

  Important change:
   CIDSystemInfo entry is changed to struct cid_sysinfo_s from int id.

  You must supply all field marked as <required>.
  If you known about panose, please supply it also in Style field.
   << /Panose <12-byte sequence> >>
*/

struct cid_info {
  NAME       fontname;     /* PostScript Font Name         <required> */
  NAME       subtype;      /* Subtype, CIDType0/CIDType2   <required> */
  struct cid_sysinfo_s cc; /* Charcter Collection          <required> */
  INT        ascent;       /* Ascent                       <required> */
  INT        capheight;    /* CapHeight                    <required> */
  INT        descent;      /* Descent                      <required> */
  INT        flags;        /* Flags                        <required> */
  INT        dw;           /* DW (Default Width)                      */
  ARRAY      w;            /* W (Width array)                         */
  ARRAY      dw2;          /* DW2 (for VWmode)                        */
  ARRAY      w2;           /* W2 (for VWmode)                         */
  ARRAY      bbox;         /* FontBBox                     <required> */ 
  INT        italic;       /* ItalicAngle                  <required> */
  INT        stemv;        /* StemV,                       <required> */
  INT        avgw;         /* AvgWidth                                */
  INT        leading;      /* Leading                                 */
  INT        maxw;         /* MaxWidth                                */
  INT        missw;        /* MissingWidth                            */
  INT        stemh;        /* StemH                                   */
  INT        xheight;      /* XHeight                                 */
  STRING     charset;      /* Charset                                 */
  DICTIONARY style;        /* Style                                   */
};

/*
  Subtypes:
   CIDFontType0 - PostScript Type1 glyph data
   CIDFontType2 - TrueType glyph data
*/
#define TYPE0 "CIDFontType0"
#define TYPE2 "CIDFontType2"

/*
  Acro fonts:
   HeiseiMin-W3-Acro, HeiseiKakuGo-W5-Acro (Japan1)
   HYGoThic-Medium, HYSMyeongJo-Medium (Korea1)
   MHei-Medium, MSung-Light (CNS1)
   STSong-Light (GB1)

  PostScript "standard" font
   Ryumin-Light, GothicBBB-Medium (Japan1)

  Font metrics for Acro fonts are that found in PDF-lib p_cid.h
*/

/* Should not use 0 to represent undefined INT values ... */
const struct cid_info cid_basefont[] = {
  {
    "Ryumin-Light",
    TYPE0,
    {"Adobe", "Japan1", 2},
    723, 709, -241,
    SYMBOLIC+SERIF,
    1000, NULL, NULL, NULL,
    "[-170 -331 1024 903]",
    0, 69,
    0, 0, 0, 0,
    0, 0,
    NULL, "<< /Panose <010502020300000000000000> >>"
  },
  {
    "GothicBBB-Medium",
    TYPE0,
    {"Adobe", "Japan1", 2},
    752, 737, -271,
    SYMBOLIC,
    1000, NULL, NULL, NULL,
    "[-174 -268 1001 944]",
    0, 99,
    0, 0, 0, 0,
    0, 0,
    NULL, "<< /Panose  <0801020b0500000000000000> >>"
  },
  {
    "HeiseiKakuGo-W5",
    TYPE0,                  /* Subtype <required> */
    {"Adobe", "Japan1", 2}, /* CIDSystemInfo <required> */
    752, 737, -221,         /* Ascent, CapHeight, Descent <required> */
    SYMBOLIC,               /* Flags <required> */
    0, NULL, NULL, NULL,    /* DW, W, DW2, W2 <optioanl> */
    "[-92 -250 1010 922]",  /* FontBBox <required> */
    0, 114,     /* ItalicAngle, StemV <required> */
    0, 0, 0, 0, /* AvgWidth, Leading, MaxWidth, MissingWidth <optional> */
    0, 553,     /* StemH, XHeight <optional> */
    NULL, NULL  /* CharSet, Style <optional> */
  },
  {
    "HeiseiMin-W3",
    TYPE0,
    {"Adobe", "Japan1", 2},
    723, 709, -241,
    SYMBOLIC+SERIF,
    0, NULL, NULL, NULL,
    "[-123 -257 1001 910]",
    0, 69,
    0, 0, 0, 0,
    0, 450,
    NULL, NULL
  },
  {
    "HYGoThic-Medium",
    TYPE0,                  /* Subtype */
    {"Adobe", "Korea1", 1}, /* CIDSystemInfo */
    752, 737, -271,         /* Ascent, CapHeight, Descent */
    SYMBOLIC,               /* Flags */
    1000, NULL, NULL, NULL, /* DW, W, DW2, W2 */
    "[ -6 -145 1003 880 ]", /* FontBBox */
    0.0, 58,    /* ItalicAngle, StemV */
    0, 0, 0, 0, /* AvgWidth, Leading, MaxWidth, MissingWidth */
    0, 533,     /* StemH, XHeight */
    NULL, NULL  /* CharSet, Style */
  },
  {
    "HYSMyeongJo-Medium",
    TYPE0,
    {"Adobe", "Korea1", 1},
    752, 737, -271,
    SERIF+SYMBOLIC,
    1000, NULL, NULL, NULL,
    "[ -0 -148 1001 880 ]",
    0.0, 58,
    0, 0, 0, 0,
    0, 553,
    NULL, NULL
  },
  {
    "MHei-Medium",
    TYPE0,
    {"Adobe", "CNS1", 0},
    752, 737, -271,
    SYMBOLIC,
    1000, NULL, NULL, NULL,
    "[ -45 -250 1015 887 ]",
    0.0, 58,
    0, 0, 0, 0,
    0, 553,
    NULL, NULL
  },
  {
    "MSung-Light",
    TYPE0,
    {"Adobe", "CNS1", 0},
    752, 737, -271,
    SERIF+SYMBOLIC,
    1000, NULL, NULL, NULL,
    "[ -160 -259 1015 888 ]",
    0.0, 58,
    0, 0, 0, 0,
    0, 553,
    NULL, NULL
  },
  {
    "STSong-Light",
    TYPE0,
    {"Adobe", "GB1", 2},
    857, 857, -143,
    SERIF+SYMBOLIC,
    1000, NULL, NULL, NULL,
    "[ -250 -143 600 857 ]",
    0.0, 91,
    0, 0, 0, 0,
    0, 599,
    NULL, NULL
  }
};

#define CID_LAST_BASEFONT sizeof(cid_basefont)/sizeof(struct cid_info)

#endif /* _CID_BASEFONT_H_ */
