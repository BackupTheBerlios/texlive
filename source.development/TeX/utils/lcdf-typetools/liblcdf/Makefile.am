## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES = liblcdf.a

liblcdf_a_SOURCES = bezier.cc \
	clp.c \
	error.cc \
	filename.cc \
	landmark.cc \
	permstr.cc \
	point.cc \
	slurper.cc \
	straccum.cc \
	string.cc \
	strtonum.c \
	transform.cc \
	vectorv.cc

liblcdf_a_DEPENDENCIES = @FIXLIBC_O@
liblcdf_a_LIBADD = @FIXLIBC_O@ @TEMPLATE_OBJS@
CLEANFILES = @TEMPLATE_OBJS@

INCLUDES = -I$(top_srcdir)/include

EXTRA_DIST = fixlibc.c

srclinks:
	test -d $(top_srcdir)/include || mkdir $(top_srcdir)/include
	test -d $(top_srcdir)/include/lcdf || mkdir $(top_srcdir)/include/lcdf
	cd $(top_srcdir); for i in bezier permstr point straccum string transform landmark error filename slurper ; do \
	ln -sf ../../liblcdf/liblcdf/$$i.cc liblcdf/$$i.cc ; \
	ln -sf ../../../liblcdf/include/lcdf/$$i.hh include/lcdf/$$i.hh ; done
	cd $(top_srcdir); for i in clp strtonum ; do \
	ln -sf ../../liblcdf/liblcdf/$$i.c liblcdf/$$i.c ; \
	ln -sf ../../../liblcdf/include/lcdf/$$i.h include/lcdf/$$i.h ; done
	cd $(top_srcdir); for i in fixlibc.c vectorv.cc ; do \
	ln -sf ../../liblcdf/liblcdf/$$i liblcdf/$$i ; done
	cd $(top_srcdir); for i in hashmap vector ; do \
	ln -sf ../../../liblcdf/include/lcdf/$$i.cc include/lcdf/$$i.cc ; \
	ln -sf ../../../liblcdf/include/lcdf/$$i.hh include/lcdf/$$i.hh ; done
	cd $(top_srcdir); for i in inttypes ; do \
	ln -sf ../../../liblcdf/include/lcdf/$$i.h include/lcdf/$$i.h ; done
