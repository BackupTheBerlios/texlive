################################################################################
#
# Makefile  : T1Utils
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/07/13 22:17:31 popineau>
#
################################################################################
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

USE_GNUW32 = 1

!include <msvc/common.mak>

DEFS = -I. -I./include $(DEFS) -D_MSDOS -DHAVE_CONFIG_H

programs = $(objdir)\t1disasm.exe \
	$(objdir)\t1asm.exe \
	$(objdir)\t1ascii.exe \
	$(objdir)\t1binary.exe \
	$(objdir)\t1unmac.exe \
	$(objdir)\t1mac.exe

manfiles = t1disasm.1 t1asm.1 t1ascii.1 t1binary.1 t1unmac.1 t1mac.1

default: all

all: $(objdir) $(programs)

$(objdir)\t1disasm.exe:	$(objdir)\clp.obj $(objdir)\t1lib.obj $(objdir)\t1disasm.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\t1asm.exe:	$(objdir)\clp.obj $(objdir)\t1lib.obj $(objdir)\t1asm.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\t1ascii.exe:	$(objdir)\clp.obj $(objdir)\t1lib.obj $(objdir)\t1ascii.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\t1binary.exe:	$(objdir)\clp.obj $(objdir)\t1lib.obj $(objdir)\t1binary.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\t1unmac.exe:	$(objdir)\clp.obj  $(objdir)\t1lib.obj $(objdir)\t1unmac.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\t1mac.exe:	$(objdir)\clp.obj $(objdir)\t1lib.obj $(objdir)\t1mac.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

!include <msvc/config.mak>

!include <msvc/install.mak>

install:: install-exec

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End: