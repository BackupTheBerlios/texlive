################################################################################
#
# Makefile  : mminstance
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/01/04 20:27:11 popineau>
#
################################################################################
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

!include <msvc/common.mak>

# Kpathsea needs to be build before 
subdirs = \
	libefont      	\
	liblcdf		\
	mmafm		\
	mmpfb		\
	cfftot1		\
	otfinfo		\
	otftotfm	\
	t1dotlessj	\
	t1lint		\
	t1testpage

default:: all

all:: config.h

!include <msvc/subdirs.mak>
!include <msvc/config.mak>

install::
	-@$(copy) glyphlist.txt $(texmf)\fonts\type1

!include <msvc/clean.mak>

distclean::
	-$(del) config.h

#
# Local Variables:
# mode: makefile
# End:
