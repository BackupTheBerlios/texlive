################################################################################
#
# Makefile  : teTeX specific
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/09/06 00:14:23 popineau>
#
################################################################################
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

USE_GNUW32 = 1
USE_GSW32 = 1
USE_KPATHSEA = 1
USE_ADVAPI = 1

!include <msvc/common.mak>

{$(texmf)\context\perltk}.pl{$(objdir)}.exe:
	$(perlcomp) $(perlcompflags) $<

{$(texmf:-development=)\context\perltk}.pl{$(objdir)}.exe:
	$(perlcomp) $(perlcompflags) $<

{}.{$(objdir)}.exe:
	$(perlcomp) $(perlcompflags) $<

#perlscript = texdoctk e2pall texi2pdf thumbpdf
docsubdir = latex
infofiles= latex.info # latex.info-1 latex.info-2 latex.info-3
htmldocfiles = latex.html
pdfdocfiles = latex.pdf
programs= \
# #	$(objdir)\dvi2fax.exe \
# #	$(objdir)\ps2frag.exe \
# #	$(objdir)\texconfig.exe \
# #	$(objdir)\dvired.exe \
# #	$(objdir)\pslatex.exe \
# 	$(objdir)\texi2html.exe \
# #	$(objdir)\allcm.exe \
# #	$(objdir)\allneeded.exe \
# #	$(objdir)\fontexport.exe \
# #	$(objdir)\fontimport.exe \
# #	$(objdir)\kpsetool.exe \
# #	$(objdir)\mkfontdesc.exe \
# #	$(objdir)\MakeTeXPK.Exe \
# #	$(objdir)\fontinst.exe \
# #	$(objdir)\rubibtex.exe \
# #	$(objdir)\rumakeindex.exe \
# #	$(objdir)\fmtutil.exe \
# #	$(objdir)\texdoc.exe \
# #	$(objdir)\texlinks.exe \
# 	$(objdir)\texexec.exe \
# 	$(objdir)\texdoctk.exe \
# 	$(objdir)\e2pall.exe \
# 	$(objdir)\epstopdf.exe \
# #	$(objdir)\texi2pdf.exe \
# 	$(objdir)\thumbpdf.exe \
# 	$(objdir)\makempy.exe \
# #	$(objdir)\mptopdf.exe \
# 	$(objdir)\texshow.exe \
# 	$(objdir)\texfind.exe \
# 	$(objdir)\texfont.exe \
# 	$(objdir)\texutil.exe \
# 	$(objdir)\updmap.exe  \
# 	$(objdir)\fdf2tan.exe \
# 	$(objdir)\makempy.exe \
# 	$(objdir)\vpe.exe \
	fontinst.bat \
#	$(objdir)\rubibtex.bat

perlscriptdir = $(texmf)\perl

tetexperlscripts = \
	e2pall.pl \
	epstopdf.pl \
	texdoctk.pl \
	texi2html.pl \
	thumbpdf.pl \
	updmap.pl

perlscripts = \
	$(texmf)\context\perltk\texexec.pl \
	$(texmf)\context\perltk\makempy.pl \
	$(texmf)\context\perltk\texshow.pl \
	$(texmf)\context\perltk\texfind.pl \
	$(texmf)\context\perltk\texfont.pl \
	$(texmf)\context\perltk\texutil.pl \
	$(texmf)\context\perltk\fdf2tan.pl \
	$(texmf)\source\latex\oberdiek\vpe.pl

# manfiles = \
# 	allec.$(manext) \
# 	einitex.$(manext) \
# 	elatex.$(manext) \
# 	evirtex.$(manext) \
# 	inimf.$(manext) \
# 	inimpost.$(manext) \
# 	iniomega.$(manext) \
# 	initex.$(manext) \
# 	kpsepath.$(manext) \
# 	kpsexpand.$(manext) \
# 	lambda.$(manext) \
# 	pdfinitex.$(manext) \
# 	pdfvirtex.$(manext) \
# 	texhash.$(manext) \
# 	virmf.$(manext) \
# 	virmpost.$(manext) \
# 	viromega.$(manext) \
# 	virtex.$(manext) \
# 	pdflatex.$(manext) \
# 	MakeTeXPK.$(manext) \
# 	cont-de.$(manext) \
# 	cont-nl.$(manext) \
# 	cont-en.$(manext)

manfiles = \
	allcm.$(manext) \
	allec.$(manext) \
	allneeded.$(manext) \
	dvi2fax.$(manext) \
	dvired.$(manext) \
	e2pall.$(manext) \
	epstopdf.$(manext) \
	fontexport.$(manext) \
	fontimport.$(manext) \
	fontinst.$(manext) \
	kpsepath.$(manext) \
	kpsetool.$(manext) \
	kpsexpand.$(manext) \
	ps2frag.$(manext) \
	pslatex.$(manext) \
	rubibtex.$(manext) \
	rumakeindex.$(manext) \
	texconfig.$(manext) \
	texdoc.$(manext) \
	texexec.$(manext) \
	texi2html.$(manext) \
#	texi2pdf.$(manext) \
	texshow.$(manext) \
	texutil.$(manext) \
	thumbpdf.$(manext) \
	texdoctk.$(manext) \
	fmtutil.8 \
	mkfontdesc.8 \
	texlinks.8 \
	fmtutil.cnf.5 

default: all

all: $(objdir) $(programs)

#$(objdir)\epstopdf.exe: $(objdir)\epstopdf.obj $(gsw32lib) $(kpathsealib)
#	$(link) $(advapiflags) $(**) $(conlibs) $(advapilibs)

e2pall.pl: e2pall
	$(sed) -e "1,2d" < $(**) > $@

epstopdf.pl: epstopdf
	$(sed) -e "1,2d" < $(**) > $@

texi2html.pl: texi2html
	$(sed) -e "1,2d" < $(**) > $@

texdoctk.pl: texdoctk
	$(sed) -e "1,2d" < $(**) > $@

# texi2pdf.pl: texi2pdf
# 	$(sed) -e "1,2d" < $(**) > $@

thumbpdf.pl: thumbpdf
	$(sed) -e "1,2d" < $(**) > $@

$(objdir)\e2pall.exe: e2pall
	$(perlcomp) $(perlcompflags) $(**)

$(objdir)\epstopdf.exe: epstopdf
	$(perlcomp) $(perlcompflags) $(**)

$(objdir)\texi2html.exe: texi2html
	$(perlcomp) $(perlcompflags) $(**)

$(objdir)\texdoctk.exe: texdoctk
	$(perlcomp) $(perlcompflags) $(**)

# $(objdir)\texi2pdf.exe: texi2pdf

$(objdir)\thumbpdf.exe: thumbpdf
	$(perlcomp) $(perlcompflags) $(**)

$(objdir)\updmap.exe: updmap.pl
	$(perlcomp) $(perlcompflags) $(**)

$(objdir)\vpe.exe: vpe.pl
	$(perlcomp) $(perlcompflags) $(**)

!include <msvc/config.mak>

installdirs = "$(perlscriptdir)"

!include <msvc/install.mak>

install:: install-exec install-man install-info install-doc

install-exec:: $(tetexperlscripts) # $(scripts) $(pdfscripts) $(runperl) $(programs)
	-@echo $(verbose) & ( \
		if not "$(tetexperlscripts)"=="" ( \
			for %%s in ($(tetexperlscripts)) do \
				$(copy) %%s $(perlscriptdir)\%%s $(redir_stdout) \
				& $(perlcomp) $(perlcompflags) %%s >> $(win32perldir)\perlfiles.lst \
				& $(runperl) -f %%~nxs $(bindir)\%%~ns.exe $(redir_stderr) \
		) \
	)
	-@echo $(verbose) & ( \
		if not "$(perlscripts)"=="" ( \
			for %%s in ($(perlscripts)) do \
				$(perlcomp) $(perlcompflags) %%s >> $(win32perldir)\perlfiles.lst \
				& $(runperl) -f %%~nxs $(bindir)\%%~ns.exe $(redir_stderr) \
		) \
	)
# 	-@$(del) $(scriptdir)\allec.* $(scriptdir)\kpsepath.* $(scriptdir)\kpsexpand.* $(scriptdir)\texhash.*
# 	  & ln -s allcm allec; \
# 	  & ln -s kpsetool kpsepath; \
# 	  & ln -s kpsetool kpsexpand;
# We should use irun instead of copy here, but there is not much space to gain.
	-@echo $(verbose) & ( \
		pushd $(scriptdir) & $(copy) .\mktexlsr.exe $(scriptdir)\texhash.exe & popd \
	) $(redir_stdout)

latex.texi: latex2e.texi
	@$(copy) $(**) $@ $(redir_stdout)

clean::
	-@for %%i in ($(scripts) $(pdfscripts)) do $(del) %%i
	-@$(del) *.pdf *.html fmtutil.cnf
	-@$(del) e2pall.pl epstopdf.pl texi2html.pl texdoctk.pl thumbpdf.pl

depend::

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"
# Local Variables:
# mode: Makefile
# End:
