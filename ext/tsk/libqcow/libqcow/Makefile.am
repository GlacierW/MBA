AM_CPPFLAGS = \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@LIBCSTRING_CPPFLAGS@ \
	@LIBCERROR_CPPFLAGS@ \
	@LIBCTHREADS_CPPFLAGS@ \
	@LIBCDATA_CPPFLAGS@ \
	@LIBCLOCALE_CPPFLAGS@ \
	@LIBCNOTIFY_CPPFLAGS@ \
	@LIBCSPLIT_CPPFLAGS@ \
	@LIBUNA_CPPFLAGS@ \
	@LIBCFILE_CPPFLAGS@ \
	@LIBCPATH_CPPFLAGS@ \
	@LIBBFIO_CPPFLAGS@ \
	@LIBFCACHE_CPPFLAGS@ \
	@LIBFDATA_CPPFLAGS@ \
	@ZLIB_CPPFLAGS@ \
	@LIBCAES_CPPFLAGS@ \
	@LIBCRYPTO_CPPFLAGS@ \
	@PTHREAD_CPPFLAGS@

lib_LTLIBRARIES = libqcow.la

libqcow_la_SOURCES = \
	libqcow.c \
	libqcow_cluster_block.c libqcow_cluster_block.h \
	libqcow_cluster_table.c libqcow_cluster_table.h \
	libqcow_codepage.h \
	libqcow_compression.c libqcow_compression.h \
	libqcow_debug.c libqcow_debug.h \
	libqcow_definitions.h \
	libqcow_deflate.c libqcow_deflate.h \
	libqcow_encryption.c libqcow_encryption.h \
	libqcow_error.c libqcow_error.h \
	libqcow_extern.h \
	libqcow_file.c libqcow_file.h \
	libqcow_i18n.c libqcow_i18n.h \
	libqcow_io_handle.c libqcow_io_handle.h \
	libqcow_libbfio.h \
	libqcow_libcaes.h \
	libqcow_libcerror.h \
	libqcow_libclocale.h \
	libqcow_libcnotify.h \
	libqcow_libcstring.h \
	libqcow_libcthreads.h \
	libqcow_libfcache.h \
	libqcow_libfdata.h \
	libqcow_libuna.h \
	libqcow_metadata.c libqcow_metadata.h \
	libqcow_notify.c libqcow_notify.h \
	libqcow_support.c libqcow_support.h \
	libqcow_types.h \
	libqcow_unused.h \
	qcow_file_header.h

libqcow_la_LIBADD = \
	@LIBCSTRING_LIBADD@ \
	@LIBCERROR_LIBADD@ \
	@LIBCTHREADS_LIBADD@ \
	@LIBCDATA_LIBADD@ \
	@LIBCLOCALE_LIBADD@ \
	@LIBCNOTIFY_LIBADD@ \
	@LIBCSPLIT_LIBADD@ \
	@LIBUNA_LIBADD@ \
	@LIBCFILE_LIBADD@ \
	@LIBCPATH_LIBADD@ \
	@LIBBFIO_LIBADD@ \
	@LIBFCACHE_LIBADD@ \
	@LIBFDATA_LIBADD@ \
	@ZLIB_LIBADD@ \
	@LIBCAES_LIBADD@ \
	@LIBCRYPTO_LIBADD@ \
	@LIBDL_LIBADD@ \
	@PTHREAD_LIBADD@

libqcow_la_LDFLAGS = -no-undefined -version-info 1:0:0

EXTRA_DIST = \
	libqcow_definitions.h.in \
	libqcow.rc \
	libqcow.rc.in

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f libqcow_definitions.h
	/bin/rm -f libqcow.rc
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libqcow ..."
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libqcow_la_SOURCES)

