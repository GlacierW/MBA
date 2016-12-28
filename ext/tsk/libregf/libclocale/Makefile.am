if HAVE_LOCAL_LIBCLOCALE
AM_CPPFLAGS = \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@LIBCERROR_CPPFLAGS@

noinst_LTLIBRARIES = libclocale.la

libclocale_la_SOURCES = \
	libclocale_codepage.c libclocale_codepage.h \
	libclocale_definitions.h \
	libclocale_extern.h \
	libclocale_libcerror.h \
	libclocale_locale.c libclocale_locale.h \
	libclocale_support.c libclocale_support.h \
	libclocale_unused.h \
	libclocale_wide_string.c libclocale_wide_string.h
endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libclocale ..."
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libclocale_la_SOURCES)

