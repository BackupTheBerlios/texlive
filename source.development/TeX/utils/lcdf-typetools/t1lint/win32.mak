################################################################################
#
# Makefile  : mminstance, t1lint program
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/07/22 05:26:03 popineau>
#
################################################################################
root_srcdir = ..\..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

USE_GNUW32 = 1

!include <msvc/common.mak>

DEFS = -I.. -I../include -DHAVE_CONFIG_H $(DEFS)

libs =  ..\libefont\$(objdir)\libefont.lib	\
	..\liblcdf\$(objdir)\liblcdf.lib

programs = $(objdir)\t1lint.exe
manfiles = t1lint.1

objects = $(objdir)\cscheck.obj $(objdir)\t1lint.obj

default: all

all: $(objdir) $(programs)

$(objdir)\t1lint.exe: $(objects) $(libs) $(gnuw32lib)
	$(link) $(**) $(conlibs)

..\config.h: ..\config.h.in
	$(perl) $(win32perldir)\conf-cauto.pl ..\config.h.in $@

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
