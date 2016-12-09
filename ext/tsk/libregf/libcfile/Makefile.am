if HAVE_LOCAL_LIBCFILE
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@LIBCERROR_CPPFLAGS@ \
	@LIBCLOCALE_CPPFLAGS@ \
	@LIBCNOTIFY_CPPFLAGS@ \
	@LIBUNA_CPPFLAGS@

noinst_LTLIBRARIES = libcfile.la

libcfile_la_SOURCES = \
	libcfile_definitions.h \
	libcfile_error.c libcfile_error.h \
	libcfile_extern.h \
	libcfile_file.c libcfile_file.h \
	libcfile_libcerror.h \
	libcfile_libclocale.h \
	libcfile_libcnotify.h \
	libcfile_libuna.h \
	libcfile_notify.c libcfile_notify.h \
	libcfile_support.c libcfile_support.h \
	libcfile_system_string.c libcfile_system_string.h \
	libcfile_types.h \
	libcfile_unused.h
endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libcfile ..."
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libcfile_la_SOURCES)

