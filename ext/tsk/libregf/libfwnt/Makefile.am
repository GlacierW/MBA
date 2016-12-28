if HAVE_LOCAL_LIBFWNT
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@LIBCERROR_CPPFLAGS@ \
	@LIBCTHREADS_CPPFLAGS@ \
	@LIBCDATA_CPPFLAGS@ \
	@LIBCNOTIFY_CPPFLAGS@ \
	@PTHREAD_CPPFLAGS@

noinst_LTLIBRARIES = libfwnt.la

libfwnt_la_SOURCES = \
	libfwnt_access_control_entry.c libfwnt_access_control_entry.h \
	libfwnt_access_control_list.c libfwnt_access_control_list.h \
	libfwnt_bit_stream.c libfwnt_bit_stream.h \
	libfwnt_debug.c libfwnt_debug.h \
	libfwnt_definitions.h \
	libfwnt_extern.h \
	libfwnt_error.c libfwnt_error.h \
	libfwnt_libcerror.h \
	libfwnt_libcdata.h \
	libfwnt_libcnotify.h \
	libfwnt_locale_identifier.c libfwnt_locale_identifier.h \
	libfwnt_lznt1.c libfwnt_lznt1.h \
	libfwnt_lzxpress.c libfwnt_lzxpress.h \
	libfwnt_security_descriptor.c libfwnt_security_descriptor.h \
	libfwnt_security_identifier.c libfwnt_security_identifier.h \
	libfwnt_support.c libfwnt_support.h \
	libfwnt_types.h \
	libfwnt_unused.h
endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libfwnt ..."
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libfwnt_la_SOURCES)

