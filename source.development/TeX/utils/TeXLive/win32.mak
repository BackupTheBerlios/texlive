################################################################################
#
# Makefile  : TeXLive
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/05/23 13:49:55 popineau>
#
################################################################################
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

USE_GETURL = 1
USE_UNZIP = 1
USE_GNUW32 = 1
USE_GSW32 = 1
USE_EXPAT = 1
USE_KPATHSEA = 1
USE_NETWORK = 1
USE_ADVAPI = 1

STATIC = 1

!include <msvc/common.mak>

make = $(make) STATIC=1

# We should install in bin/win32 anyway
bindir = $(exec_prefix)\bin\win32

{..\..\texk\kpathsea}.c{$(objdir)}.obj:
	$(compile) $<

{..\TeXSetup}.cpp{$(objdir)}.obj:
	$(compile) $<

DEFS = $(DEFS:-D__STDC__=-U__STDC__) -D "WIN32" -D "_WINDOWS" \
	-D "_MBCS" -EHsc -Zc:wchar_t -W3 -nologo -Wp64 \
	-I../TeXSetup
DEFS = $(DEFS:-DWIN32_LEAN_AND_MEAN=)

!ifdef DEBUG
DEFS = $(DEFS) -MTd -D "_DEBUG"
!else
DEFS = $(DEFS) -MT
!endif

#
# Main target
#
objects = \
	$(objdir)\ActionProgressDlg.obj \
	$(objdir)\ColorStaticST.obj \
	$(objdir)\KeyValFile.obj \
	$(objdir)\ListChoiceDlg.obj \
	$(objdir)\Registry.obj \
	$(objdir)\StdAfx.obj \
	$(objdir)\TeXLive.obj \
	$(objdir)\TeXLiveCmds.obj \
	$(objdir)\TeXLiveDlg.obj \
	$(objdir)\Win32Util.obj

programs = $(objdir)\TeXLive.exe

htmlhelplib = "c:\Program Files\HTML Help WorkShop\lib\htmlhelp.lib"

default: all

all:: $(objdir) $(programs)

$(objdir)\TeXLive.exe: $(objects) $(objdir)\TeXLive.res $(htmlhelplib) $(unziplib) $(gsw32lib) $(gnuw32lib)
	$(link_gui) $(**) atls.lib nafxcw.lib  $(guilibs) ole32.lib oleaut32.lib oledlg.lib oleacc.lib uuid.lib shlwapi.lib shell32.lib version.lib /delayload:oleacc.dll /delayload:oledlg.dll

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec

clean::
	-@$(del) $(OUTDIR)\*

distclean::
	-@$(del) Release\* Debug\* *.aps *.ncb *.opt

!include <msvc/clean.mak>

depend:

#
# Local Variables:
# mode: makefile
# End:
