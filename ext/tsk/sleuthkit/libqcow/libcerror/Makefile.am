if HAVE_LOCAL_LIBCERROR
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common

noinst_LTLIBRARIES = libcerror.la

libcerror_la_SOURCES = \
	libcerror_definitions.h \
	libcerror_extern.h \
	libcerror_error.c libcerror_error.h \
	libcerror_support.c libcerror_support.h \
	libcerror_system.c libcerror_system.h \
	libcerror_types.h \
	libcerror_unused.h
endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libcerror ..."
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libcerror_la_SOURCES)

