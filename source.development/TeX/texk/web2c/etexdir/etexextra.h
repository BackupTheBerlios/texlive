/* etexextra.h: banner etc. for e-TeX.

   This is included by e-TeX, from ../etexextra.c
   (generated from ../lib/texmfmp.c).

Copyright (C) 1995, 96 Karl Berry.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define BANNER "This is e-TeX, Version 3.141592-2.1"
#define COPYRIGHT_HOLDER "The NTS Team"
#define AUTHOR NULL
#define PROGRAM_HELP ETEXHELP
#define DUMP_VAR TEXformatdefault
#define DUMP_LENGTH_VAR formatdefaultlength
#define DUMP_OPTION "efmt"
#ifdef DOS
#define DUMP_EXT ".efm"
#else
#define DUMP_EXT ".efmt"
#endif
#define INPUT_FORMAT kpse_tex_format
#define INI_PROGRAM "einitex"
#define VIR_PROGRAM "evirtex"
