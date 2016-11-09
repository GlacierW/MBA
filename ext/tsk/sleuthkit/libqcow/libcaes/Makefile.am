if HAVE_LOCAL_LIBCAES
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@LIBCERROR_CPPFLAGS@

noinst_LTLIBRARIES = libcaes.la

libcaes_la_SOURCES = \
	libcaes_context.c libcaes_context.h \
	libcaes_definitions.h \
	libcaes_extern.h \
	libcaes_error.c libcaes_error.h \
	libcaes_key.c libcaes_key.h \
	libcaes_libcerror.h \
	libcaes_support.c libcaes_support.h \
	libcaes_tweaked_context.c libcaes_tweaked_context.h \
	libcaes_types.h \
	libcaes_unused.h
endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libcaes ..."
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libcaes_la_SOURCES)

