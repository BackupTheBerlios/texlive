################################################################################
#
# Makefile  : Dvipdfmx
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.7 sources
# Time-stamp: <03/02/23 13:44:45 popineau>
#
################################################################################
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

!include <msvc/common.mak>

# Package subdirectories, the library, and all subdirectories.
subdirs = crypto src

default:: all

all:: config.h

!include <msvc/subdirs.mak>
!include <msvc/config.mak>
!include <msvc/clean.mak>

# Local Variables:
# mode: Makefile
# End:
