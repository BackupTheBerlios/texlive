################################################################################
#
# Makefile  : paths definitions for Xemtex tools
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/07/05 23:19:37 popineau>
#
################################################################################


xemtexbin = $(texprefix)\bin\win32

prefix = $(xemtextools)
bindir = $(xemtexbin)
#libdir = $(prefix)\lib
#sharedir = $(prefix)\share
#includedir = $(prefix)\include
docdir = $(xemtextools)\doc
mandir = $(xemtextools)\man
infodir = $(xemtextools)\info
htmldocdir = $(docdir)\html

# End of xemtexpaths.mak
# 
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
