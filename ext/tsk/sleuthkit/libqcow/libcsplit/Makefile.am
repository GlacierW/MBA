if HAVE_LOCAL_LIBCSPLIT
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@LIBCERROR_CPPFLAGS@

noinst_LTLIBRARIES = libcsplit.la

libcsplit_la_SOURCES = \
	libcsplit_definitions.h \
	libcsplit_error.c libcsplit_error.h \
	libcsplit_extern.h \
	libcsplit_narrow_split_string.c libcsplit_narrow_split_string.h \
	libcsplit_narrow_string.c libcsplit_narrow_string.h \
	libcsplit_libcerror.h \
	libcsplit_support.c libcsplit_support.h \
	libcsplit_types.h \
	libcsplit_unused.h \
	libcsplit_wide_split_string.c libcsplit_wide_split_string.h \
	libcsplit_wide_string.c libcsplit_wide_string.h
endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libcsplit ..."
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libcsplit_la_SOURCES)

