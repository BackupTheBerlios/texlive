################################################################################
#
# Makefile  : TeXk / CJKutils / hbf2gf
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/07/24 08:40:32 popineau>
#
################################################################################
root_srcdir = ..\..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

# This is free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License, see the file COPYING.

USE_KPATHSEA = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

#
# Files
#
programs = $(objdir)\hbf2gf.exe
objects = $(objdir)\hbf.obj $(objdir)\hbf2gf.obj
manfiles = hbf2gf.1

DEFS = $(DEFS) -DHAVE_LIBKPATHSEA -Dmsdos
# for FAT support, set: DEFS = -DNTFAT

.SUFFIXES: .w .c .obj .res .rc
.w.c:
	 ..\..\web2c\$(objdir)\ctangle $*

#
# Main target
#
default: all

all: $(objdir) $(programs)

#
# Link target. setargv.obj is provided in the compiler library directory.
#
$(objdir)\hbf2gf.exe: $(objects) $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\hbf2gf.obj: hbf2gf.c

!include <msvc/config.mak>

installdirs = $(texmf)\hbf2gf

!include <msvc/install.mak>

install:: install-exec install-man install-data

install-data::
	-@echo $(verbose) & ( \
		 $(xcopy) cfg\*.* $(texmf)\hbf2gf \
	)

!include <msvc/clean.mak>

distclean::
	-@$(del) hbf2gf.c

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
