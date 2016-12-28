if HAVE_LOCAL_LIBFDATETIME
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@LIBCERROR_CPPFLAGS@

noinst_LTLIBRARIES = libfdatetime.la

libfdatetime_la_SOURCES = \
	libfdatetime_date_time_values.c libfdatetime_date_time_values.h \
	libfdatetime_definitions.h \
	libfdatetime_extern.h \
	libfdatetime_error.c libfdatetime_error.h \
	libfdatetime_fat_date_time.c libfdatetime_fat_date_time.h \
	libfdatetime_filetime.c libfdatetime_filetime.h \
	libfdatetime_floatingtime.c libfdatetime_floatingtime.h \
	libfdatetime_hfs_time.c libfdatetime_hfs_time.h \
	libfdatetime_libcerror.h \
	libfdatetime_nsf_timedate.c libfdatetime_nsf_timedate.h \
	libfdatetime_posix_time.c libfdatetime_posix_time.h \
	libfdatetime_support.c libfdatetime_support.h \
	libfdatetime_systemtime.c libfdatetime_systemtime.h \
	libfdatetime_types.h \
	libfdatetime_unused.h
endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libfdatetime ..."
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libfdatetime_la_SOURCES)

