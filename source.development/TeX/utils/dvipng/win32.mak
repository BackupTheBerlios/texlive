################################################################################
#
# Makefile  : Dvipng
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/05/13 22:15:04 popineau>
#
################################################################################
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

USE_GNUW32 = 1
USE_DVIPNG = 1
MAKE_DVIPNG = 1

!include <msvc/common.mak>

programs = $(objdir)\dvipng.exe
includefiles = 
manfiles = 
infofiles = 
installinfoflags = 

objects= $(objdir)\color.obj  \
      $(objdir)\draw.obj      \
      $(objdir)\dvi.obj       \
      $(objdir)\dvipng.obj    \
      $(objdir)\font.obj      \
      $(objdir)\misc.obj      \
      $(objdir)\papersiz.obj  \
      $(objdir)\pk.obj        \
      $(objdir)\ppagelist.obj \
      $(objdir)\set.obj       \
      $(objdir)\special.obj   \
      $(objdir)\vf.obj

default: all

all: $(objdir) $(programs)

$(objdir)\dvipng.exe: $(objects)
	$(link) $(**) $(conlibs)

!include <msvc/config.mak>
!include <msvc/install.mak>

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End: