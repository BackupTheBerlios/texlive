# This file is part of the FreeType project.
#
# It builds the library for BeOS with gcc.
#
# You will need GNU make.
#
# Use this file while in the lib directory with the following statement:
#
#   make -f arch/beos/Makefile.gcc
#
# If you have the GNU gettext package installed, you can also try
#
#   make -f arch/beos/Makefile.gcc HAVE_GETTEXT
#
#
# Copyright 1996-2001 by
# David Turner, Robert Wilhelm, and Werner Lemberg.
#
# This file is part of the FreeType project, and may only be used, modified,
# and distributed under the terms of the FreeType project license,
# LICENSE.TXT.  By continuing to use, modify, or distribute this file you
# indicate that you have read the license and understand and accept it
# fully.

ARCH = arch/beos
FT_MAKEFILE = $(ARCH)/Makefile.gcc

CC = gcc

ifndef GETTEXT
GETTEXT=NO_GETTEXT
endif

ifdef DEBUG
CFLAGS = -Wall -O2 -g -ansi -pedantic -I$(ARCH) -I. -Iextend -D$(GETTEXT)
else
CFLAGS = -Wall -ansi -pedantic -O2 -s -I$(ARCH) -I. -Iextend -D$(GETTEXT)
endif


TTFILE = ./ttfile.c
TTMEMORY = ./ttmemory.c
TTMUTEX = $(ARCH)/ttmutex.c

PORT = $(TTFILE) $(TTMEMORY) $(TTMUTEX)

SRC_X = extend/ftxgasp.c extend/ftxkern.c  extend/ftxpost.c \
        extend/ftxcmap.c extend/ftxwidth.c extend/ftxsbit.c \
        extend/ftxgsub.c extend/ftxgpos.c  extend/ftxgdef.c \
        extend/ftxopen.c extend/ftxerr18.c
OBJS_X = $(SRC_X:.c=.o)

SRC_M = ttapi.c     ttcache.c   ttcalc.c   ttcmap.c  ttdebug.c \
        ttgload.c   ttinterp.c  ttload.c   ttobjs.c  \
        ttraster.c  ttextend.c  $(PORT)
OBJS_M = $(SRC_M:.c=.o) $(OBJS_X)

SRC_S = $(ARCH)/freetype.c
OBJ_S = $(SRC_S:.c=.o)
OBJS_S = $(OBJ_S) $(OBJS_X)


%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: all debug clean distclean depend


all:
	$(MAKE) -f $(FT_MAKEFILE) LIB_FILES=OBJS_S libttf.a

debug:
	$(MAKE) -f $(FT_MAKEFILE) LIB_FILES=OBJS_M DEBUG=1 libttf.a

HAVE_GETTEXT:
	$(MAKE) -f $(FT_MAKEFILE) LIB_FILES=OBJS_S GETTEXT=HAVE_GETTEXT \
                libttf.a

$(OBJ_S): $(SRC_S) $(SRC_M)

libttf.a: $($(LIB_FILES))
	-del $@
	ar src $@ $^

clean:
	-del *.o
	-del extend\*.o
	-del $(ARCH)\*.o
	-del response

distclean: clean
	-del dep.end
	-del libttf.a

depend: $(SRC_S) $(SRC_M) $(SRC_X)
	$(CC) -E -MM -I$(ARCH) -I. -Iextend $^ > dep.end

ifeq (dep.end,$(wildcard dep.end))
  include dep.end
endif

# end of Makefile.gcc
