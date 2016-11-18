if HAVE_LOCAL_LIBFCACHE
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@LIBCERROR_CPPFLAGS@ \
	@LIBCTHREADS_CPPFLAGS@ \
	@LIBCDATA_CPPFLAGS@ \
	@PTHREAD_CPPFLAGS@

noinst_LTLIBRARIES = libfcache.la

libfcache_la_SOURCES = \
	libfcache_cache.c libfcache_cache.h \
	libfcache_cache_value.c libfcache_cache_value.h \
	libfcache_date_time.c libfcache_date_time.h \
	libfcache_definitions.h \
	libfcache_error.c libfcache_error.h \
	libfcache_extern.h \
	libfcache_libcdata.h \
	libfcache_libcerror.h \
	libfcache_support.c libfcache_support.h \
	libfcache_types.h \
	libfcache_unused.h
endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libfcache ..."
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libfcache_la_SOURCES)

