################################################################################
#
# Makefile  : TeXLive specific
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/09/25 22:51:18 popineau>
#
################################################################################
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
# End of generated configuration data.
# Makefile for texlive's extra scripts 
# Public domain.

!include <msvc/common.mak>

{$(texmf)\context\perltk}.pl{$(objdir)}.exe:
	$(perlcomp) $(perlcompflags) $<

{$(texmf:-development=)\context\perltk}.pl{$(objdir)}.exe:
	$(perlcomp) $(perlcompflags) $<

{}.pl{$(objdir)}.exe:
	$(perlcomp) $(perlcompflags) $<

{}.{$(objdir)}.exe:
	$(perlcomp) $(perlcompflags) $<

perlscriptdir = $(texmf)\perl

LIVEMFPROGS = \
#	mfw.exe

LIVEMPOSTPROGS = \
	metafun.exe

LIVETEXPROGS = \
	amstex.exe \
#	bamstex.exe \
#	blue.exe \
#	bplain.exe \
#	cyrtexinfo.exe \
#	cyramstex.exe \
#	cyrlatex.exe \
#	cyrtex.exe \
	jadetex.exe \
#	lamstex.exe \
#	lollipop.exe \
	mex.exe \
	mltex.exe \
	physe.exe \
	phyzzx.exe \
	platex.exe \
#	startex.exe \
	texinfo.exe \
#	text1.exe \
	texsis.exe

LIVEETEXPROGS = \
#	elatex.exe \
	eplain.exe \
	mllatex.exe \
	latex.exe \
	xmltex.exe

LIVEPDFTEXPROGS = \
	pdfjadetex.exe \
	pdfmex.exe \
	pdfplatex.exe \
	pdftexinfo.exe

LIVEPDFETEXPROGS = \
	context.exe \
	pdfemex.exe \
#	pdfelatex.exe \
	pdflatex.exe \
	pdfxmltex.exe

LIVEEOMEGAPROGS = \
	lambda.exe

LIVESCRIPTS = \
#	cont-de.bat \
#	cont-en.bat \
#	cont-nl.bat \
#	cont-uk.bat \
#	cont-cz.bat \
#	cont-ro.bat \
	cslatex.bat \
	cslatexd.bat \
	cslatexi.bat \
	cslatexk.bat \
	csplain.bat \
	csplaind.bat \
	csplaini.bat \
	csplaink.bat \
	mex-pl.bat \
	pdfmex-pl.bat \
	pdfemex-pl.bat \
	pdfcslatex.bat \
	pdfcslatexd.bat \
	pdfcslatexi.bat \
	pdfcslatexk.bat \
	pdfcsplain.bat \
	pdfcsplaind.bat \
	pdfcsplaini.bat \
	pdfcsplaink.bat \
	platex-pl.bat \
	pdfplatex-pl.bat

LIVEPERLSCRIPTS = \
	$(texmf)\source\fonts\pkfix\pkfix.pl \
#	$(objdir)\nts.exe \
	$(texmf)\context\perltk\mptopdf.pl

LIVEJAVASCRIPTS = \
	pdfthumb \
	ppower4

baseprograms = \
	$(bindir)\tex.exe \
	$(bindir)\etex.exe \
	$(bindir)\pdftex.exe \
	$(bindir)\pdfetex.exe \
	$(bindir)\omega.exe \
	$(bindir)\eomega.exe \
	$(bindir)\mf.exe \
	$(bindir)\mpost.exe

all:  $(objdir) $(LIVEPERLSCRIPTS) $(LIVESCRIPTS) # $(baseprograms)

default: all

# pkfix.pl: pkfix
# 	$(sed) -e "1,2d" < $** > $@

# nts.pl: $(texmf)\nts\perltk\nts.pl
# 	@$(copy) $(**) $@

$(objdir)\pkfix.exe: pkfix
	$(perlcomp) $(perlcompflags) $(**)

$(objdir)\nts.exe: $(texmf:-development=)\nts\perltk\nts.pl
	$(perlcomp) $(perlcompflags) $(**)

installdirs = "$(perlscriptdir)"

!include <msvc/install.mak>

install:: install-exec

install-exec:: $(LIVEPERLSCRIPTS) $(LIVESCRIPTS) $(baseprograms)
	-@echo $(verbose) & ( \
		for %%s in ($(LIVETEXPROGS)) do \
			$(del) $(bindir)\%%s \
			& pushd $(bindir) & $(lnexe) .\tex.exe $(bindir)\%%s \
	)
	-@echo $(verbose) & ( \
		for %%s in ($(LIVEETEXPROGS)) do \
			$(del) $(bindir)\%%s \
			& pushd $(bindir) & $(lnexe) .\etex.exe $(bindir)\%%s \
	)
	-@echo $(verbose) & ( \
		for %%s in ($(LIVEPDFTEXPROGS)) do \
			$(del) $(bindir)\%%s \
			& pushd $(bindir) & $(lnexe) .\pdftex.exe $(bindir)\%%s \
	)
	-@echo $(verbose) & ( \
		for %%s in ($(LIVEPDFETEXPROGS)) do \
			$(del) $(bindir)\%%s \
			& pushd $(bindir) & $(lnexe) .\pdfetex.exe $(bindir)\%%s \
	)
	-@echo $(verbose) & ( \
		for %%s in ($(LIVEMFPROGS)) do \
			$(del) $(bindir)\%%s \
			& pushd $(bindir) & $(lnexe) .\mf.exe $(bindir)\%%s \
	)
	-@echo $(verbose) & ( \
		for %%s in ($(LIVEMPOSTPROGS)) do \
			$(del) $(bindir)\%%s \
			& pushd $(bindir) & $(lnexe) .\mpost.exe $(bindir)\%%s \
	)
	-@echo $(verbose) & ( \
		for %%s in ($(LIVEEOMEGAPROGS)) do \
			$(del) $(bindir)\%%s \
			& pushd $(bindir) & $(lnexe) .\eomega.exe $(bindir)\%%s \
	)
	-@echo $(verbose) & if NOT "$(LIVESCRIPTS)"=="" for %%s in ($(LIVESCRIPTS)) do \
	  $(copy) $(srcdir)\%%s $(scriptdir)\%%s
# 	-@echo $(verbose) & if NOT "$(LIVEPERLSCRIPTS)"=="" for %%s in ($(LIVEPERLSCRIPTS)) do \
# 	  $(copy) $(srcdir)\%%s $(scriptdir)\%%~nxs
	-@echo $(verbose) & ( \
		if not "$(LIVEPERLSCRIPTS)"=="" ( \
			for %%s in ($(LIVEPERLSCRIPTS)) do \
#				$(perlcomp) $(perlcompflags) %%s >> $(win32perldir)\perlfiles.lst \
#				& $(copy) %%s $(perlscriptdir)\%%s $(redir_stdout) \
#				& $(runperl) -f %%s $(bindir)\%%~ns.exe $(redir_stderr) \
			$(perlcomp) $(perlcompflags) %%s >> $(win32perldir)\perlfiles.lst \
			& $(runperl) -f %%~nxs $(bindir)\%%~ns.exe $(redir_stderr) \
		) \
	)
	-@echo $(verbose) & ( \
		$(runperl) -f pp4p.jar $(bindir)\ppower4.exe de.tu_darmstadt.sp.pp4.PPower4 \
		& $(runperl) -f pp4p.jar $(bindir)\pdfthumb.exe de.tu_darmstadt.sp.pdftools.ThumbGen \
	)

!include <msvc/man.mak>
!include <msvc/clean.mak>

depend:

# Local Variables:
# mode: Makefile
# End:

