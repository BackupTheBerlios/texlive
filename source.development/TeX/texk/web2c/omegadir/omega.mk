# Makefile fragment for Omega and web2c. --infovore@xs4all.nl. Public domain.
# This fragment contains the parts of the makefile that are most likely to
# differ between releases of Omega.

Makefile: omegadir/omega.mk

omegafonts_programs = omegafonts/omfonts
otps_programs = otps/otp2ocp otps/outocp
omegafonts = omegafonts
otps = otps

odvicopy = odvicopy
odvitype = odvitype
otangle = otangle
omegaware_c = odvicopy.c odvitype.c otangle.c
omegaware_programs = $(otangle) $(odvicopy) $(odvitype)

omega_programs = $(omega) $(otangle) $(odvicopy) $(odvitype) \
                 $(otps_programs) $(omegafonts_programs)

# Omegaware

odvicopy: odvicopy.o
	$(kpathsea_link) odvicopy.o $(LOADLIBES)
odvicopy.c odvicopy.h: $(web2c_common) $(web2c_programs) odvicopy.p
	$(web2c) odvicopy
odvicopy.p: tangle omegaware/odvicopy.web omegaware/odvicopy.ch
	WEBINPUTS=$(srcdir)/omegaware ./tangle odvicopy odvicopy
check: odvicopy-check
odvicopy-check: odvicopy
#	./odvicopy $(srcdir)/tests/story tests/xstory.dvi
## Redirect stderr so the terminal output will end up in the log file.
#	(TFMFONTS=$(srcdir)/tests: ./odvicopy <$(srcdir)/tests/pplr.dvi \
#	  >tests/xpplr.dvi) 2>&1
clean:: odvicopy-clean
odvicopy-clean:
	$(LIBTOOL) --mode=clean $(RM) odvicopy
	rm -f odvicopy.o odvicopy.c odvicopy.h odvicopy.p
#	rm -f tests/xstory.dvi tests/xpplr.dvi

odvitype: odvitype.o
	$(kpathsea_link) odvitype.o $(LOADLIBES)
odvitype.c odvitype.h: $(web2c_common) $(web2c_programs) odvitype.p
	$(web2c) odvitype
odvitype.p: tangle omegaware/odvitype.web omegaware/odvitype.ch
	WEBINPUTS=$(srcdir)/omegaware ./tangle odvitype odvitype
check: odvitype-check
odvitype-check: odvitype
#	./odvitype -show-opcodes $(srcdir)/tests/story >tests/xstory.dvityp
#	./odvitype --p=\*.\*.2 $(srcdir)/tests/pagenum.dvi >tests/xpagenum.typ
clean:: odvitype-clean
odvitype-clean:
	$(LIBTOOL) --mode=clean $(RM) odvitype
	rm -f odvitype.o odvitype.c odvitype.h odvitype.p
#	rm -f tests/xstory.dvityp tests/xpagenum.typ

otangle: otangle.o
	$(kpathsea_link) otangle.o $(LOADLIBES)
otangle.c otangle.h: $(web2c_common) $(web2c_programs) otangle.p
	$(web2c) otangle
otangle.p: tangle omegaware/otangle.web omegaware/otangle.ch
	WEBINPUTS=$(srcdir)/omegaware ./tangle otangle otangle
check: otangle-check
otangle-check: otangle
#	otangling omega is a good check already
clean:: otangle-clean
otangle-clean:
	$(LIBTOOL) --mode=clean $(RM) otangle
	rm -f otangle.o otangle.c otangle.h otangle.p


# Omega itself.

# We build omega
omega = omega

# The C sources.
omega_c = omegaini.c omega0.c omega1.c omega2.c omega3.c
omega_o = omegaini.o omega0.o omega1.o omega2.o \
	      omegaextra.o omega.o omegabis.o \
              omega3.o

# Linking
omega: $(omega_o)
	$(kpathsea_link) $(omega_o) $(socketlibs) $(LOADLIBES)

# The C files
$(omega_c) omegacoerce.h omegad.h: omega.p $(web2c_texmf)
	$(web2c) omega
omegaextra.c: lib/texmfmp.c
	sed s/TEX-OR-MF-OR-MP/omega/ $(srcdir)/lib/texmfmp.c >$@
# Additional C files, just copy them.
omega.c: omegadir/omega.c
	-rm -f $@
	cp $(srcdir)/omegadir/omega.c $@
omegabis.c: omegadir/omegabis.c 
	-rm -f $@
	cp $(srcdir)/omegadir/omegabis.c $@
omegamem.h: omegadir/omegamem.h
	-rm -f $@
	cp $(srcdir)/omegadir/omegamem.h $@

# The Pascal file
omega.p omega.pool: otangle omega.web omega.ch
	./otangle omega.web omega.ch

# Generation of the web and ch files.
omega.web: tie tex.web omegadir/omega.mk
omega.web: omegadir/omnode.ch
omega.web: omegadir/om16bit.ch
omega.web: omegadir/omstr.ch
omega.web: omegadir/omfont.ch
omega.web: omegadir/omchar.ch
omega.web: omegadir/omfi.ch
omega.web: omegadir/ompar.ch
omega.web: omegadir/omocp.ch
omega.web: omegadir/omfilter.ch
omega.web: omegadir/omtrans.ch
omega.web: omegadir/omdir.ch
omega.web: omegadir/omxml.ch
	./tie -m omega.web $(srcdir)/tex.web \
	 $(srcdir)/omegadir/omnode.ch \
	 $(srcdir)/omegadir/om16bit.ch \
	 $(srcdir)/omegadir/omstr.ch \
	 $(srcdir)/omegadir/omfont.ch \
	 $(srcdir)/omegadir/omchar.ch \
	 $(srcdir)/omegadir/omfi.ch \
	 $(srcdir)/omegadir/ompar.ch \
	 $(srcdir)/omegadir/omocp.ch \
	 $(srcdir)/omegadir/omfilter.ch \
	 $(srcdir)/omegadir/omtrans.ch \
	 $(srcdir)/omegadir/omdir.ch \
	 $(srcdir)/omegadir/omxml.ch
omega.ch: tie omega.web omegadir/omega.mk
omega.ch: omegadir/comnode.ch
omega.ch: omegadir/com16bit.ch
omega.ch: omegadir/comstr.ch
omega.ch: omegadir/comfont.ch
omega.ch: omegadir/comchar.ch
omega.ch: omegadir/comfi.ch
omega.ch: omegadir/compar.ch
omega.ch: omegadir/comocp.ch
omega.ch: omegadir/comfilter.ch
omega.ch: omegadir/comtrans.ch
omega.ch: omegadir/comdir.ch
omega.ch: omegadir/comxml.ch
omega.ch: omegadir/comsrcspec.ch
	./tie -c omega.ch omega.web \
	 $(srcdir)/omegadir/comnode.ch \
	 $(srcdir)/omegadir/com16bit.ch \
	 $(srcdir)/omegadir/comstr.ch \
	 $(srcdir)/omegadir/comfont.ch \
	 $(srcdir)/omegadir/comchar.ch \
	 $(srcdir)/omegadir/comfi.ch \
	 $(srcdir)/omegadir/compar.ch \
	 $(srcdir)/omegadir/comocp.ch \
	 $(srcdir)/omegadir/comfilter.ch \
	 $(srcdir)/omegadir/comtrans.ch \
	 $(srcdir)/omegadir/comdir.ch \
	 $(srcdir)/omegadir/comxml.ch \
	 $(srcdir)/omegadir/comsrcspec.ch
# Check: right now all we do is build the format.
check: omega-check
omega-check: omega omega.oft
# Cleaning up from building omega
clean:: omega-clean
omega-clean:
	$(LIBTOOL) --mode=clean $(RM) omega
	rm -f $(omega_c) omegaextra.c omegacoerce.h omegad.h
	rm -f omega.c omegabis.c omegamem.h
	rm -f omega.p omega.pool omega.web omega.ch


# Dumps.
all_ofmts = omega.oft $(ofmts)

dumps: ofmts
ofmts: $(all_ofmts)

omega.oft: omega
	$(dumpenv) $(MAKE) progname=omega files="omega.tex" prereq-check
	$(dumpenv) ./omega --progname=omega --jobname=omega --ini \\input omega.tex \\dump </dev/null

lambda.oft: omega
	$(dumpenv) $(MAKE) progname=lambda files="lambda.tex" prereq-check
	$(dumpenv) ./omega --progname=lambda --jobname=lambda --ini \\input lambda.tex </dev/null



# Some additional programs for Omega: the programs themselves are named
# in the variable otps_programs, defined above.
otps/otp2ocp:
	cd otps && $(MAKE) $(common_makeargs) otp2ocp
otps/outocp:
	cd otps && $(MAKE) $(common_makeargs) outocp
omegafonts/omfonts:
	cd omegafonts && $(MAKE) $(common_makeargs) omfonts

# Installation.
install-omega: install-omega-exec install-omega-data
install-omega-exec: install-omega-links
install-omega-data:: install-omega-dumps
install-omega-dumps: install-omega-fmts

# The actual binary executables and pool files.
install-programs: install-omega-programs
install-omega-programs: $(omega_programs) $(bindir)
	for p in omega; do $(INSTALL_LIBTOOL_PROG) $$p $(bindir); done
	cd otps && $(MAKE) $(install_makeargs) install-programs
	cd omegafonts && $(MAKE) $(install_makeargs) install-programs

install-links: install-omega-links
install-omega-links: install-omega-programs
	cd omegafonts && $(MAKE) $(install_makeargs) install-links
	cd $(bindir) && (rm -f iniomega viromega; \
	  $(LN) omega iniomega; $(LN) omega viromega)
	ofmts="$(ofmts)";
	  for f in $$ofmts; do base=`basename $$f .oft`; \
	    (cd $(bindir) && (rm -f $$base; $(LN) omega $$base)); done

# Always do plain.*, so examples from the TeXbook (etc.) will work.
install-fmts: install-omega-fmts
install-omega-fmts: ofmts $(fmtdir)
	ofmts="$(all_ofmts)";
	  for f in $$ofmts; do $(INSTALL_DATA) $$f $(fmtdir)/$$f; done

# Auxiliary files.
install-data install-omega-data:: $(texpooldir)
	$(INSTALL_DATA) omega.pool $(texpooldir)/omega.pool

# end of omega.mk
