################################################################################
#
# Makefile  : TeXSetup
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/05/23 13:48:28 popineau>
#
################################################################################
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

USE_GETURL = 1
USE_UNZIP = 1
USE_EXPAT = 1
USE_KPATHSEA = 1
USE_GNUW32 = 1
USE_GSW32 = 1
USE_INET = 1
USE_ADVAPI = 1

STATIC = 1

!include <msvc/common.mak>

make = $(make) STATIC=1

# We should install in bin/win32 anyway
bindir = $(exec_prefix)\bin\win32

{..\..\texk\kpathsea}.c{$(objdir)}.obj:
	$(compile) $<

DEFS = $(DEFS:-D__STDC__=-U__STDC__) -D "WIN32" -D "_WINDOWS" \
	-D "_MBCS" -EHsc -W3 -nologo -Zc:wchar_t -Wp64

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
	$(objdir)\ConfigPage.obj		\
	$(objdir)\Environment.obj		\
	$(objdir)\ErrorPage.obj			\
	$(objdir)\FileCopyPage.obj		\
	$(objdir)\FileDirectory.obj		\
	$(objdir)\getopt.obj			\
	$(objdir)\getopt1.obj			\
	$(objdir)\GetTPMPage.obj		\
	$(objdir)\KeyValFile.obj		\
	$(objdir)\ListChoiceDlg.obj		\
	$(objdir)\Log.obj			\
	$(objdir)\PackagesPage.obj		\
	$(objdir)\RebootPage.obj		\
	$(objdir)\Registry.obj			\
	$(objdir)\ReviewPage.obj		\
	$(objdir)\RootPage.obj			\
	$(objdir)\SchemePage.obj			\
	$(objdir)\ShellLinkData.obj		\
	$(objdir)\SourcePage.obj		\
	$(objdir)\StdAfx.obj			\
	$(objdir)\texinstall.obj		\
	$(objdir)\TeXSetup.obj			\
	$(objdir)\TeXSetupCommandLineInfo.obj	\
	$(objdir)\TeXSetupWizard.obj		\
	$(objdir)\TPM.obj			\
	$(objdir)\TPMspecial.obj		\
	$(objdir)\TransBmp.obj			\
	$(objdir)\TreePackage.obj		\
	$(objdir)\UninstallPage.obj		\
	$(objdir)\WelcomePage.obj		\
	$(objdir)\Win32Util.obj

htmlhelplib = "c:\Program Files\HTML Help WorkShop\lib\htmlhelp.lib"

programs = $(objdir)\TeXSetup.exe # $(objdir)\conspawn.exe

default: all

all:: $(objdir) $(programs)

$(objdir)\TeXSetup.exe: $(objects) $(objdir)\TeXSetup.res $(htmlhelplib) $(gnuw32lib) $(gsw32lib) $(geturllib) $(unziplib) $(expatlib)
	$(link_gui) $(**) atls.lib nafxcw.lib  $(guilibs) ole32.lib oleaut32.lib oledlg.lib oleacc.lib uuid.lib shlwapi.lib shell32.lib version.lib /delayload:oleacc.dll /delayload:oledlg.dll

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec
	@echo $(verbose) & ( $(copy) getfptex.bat $(ftpdir)\$(fptexversion) $(redir_stdout) )

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
