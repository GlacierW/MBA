AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
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
	@LIBFDATETIME_CPPFLAGS@ \
	@LIBFWNT_CPPFLAGS@ \
	@PTHREAD_CPPFLAGS@

lib_LTLIBRARIES = libregf.la

libregf_la_SOURCES = \
	libregf.c \
	libregf_checksum.c libregf_checksum.h \
	libregf_codepage.h \
	libregf_data_type.c libregf_data_type.h \
	libregf_debug.c libregf_debug.h \
	libregf_definitions.h \
	libregf_error.c libregf_error.h \
	libregf_extern.h \
	libregf_file.c libregf_file.h \
	libregf_hive_bin.c libregf_hive_bin.h \
	libregf_hive_bin_cell.c libregf_hive_bin_cell.h \
	libregf_hive_bins_list.c libregf_hive_bins_list.h \
	libregf_io_handle.c libregf_io_handle.h \
	libregf_key.c libregf_key.h \
	libregf_key_item.c libregf_key_item.h \
	libregf_key_tree.c libregf_key_tree.h \
	libregf_libbfio.h \
	libregf_libcdata.h \
	libregf_libcerror.h \
	libregf_libclocale.h \
	libregf_libcnotify.h \
	libregf_libfcache.h \
	libregf_libfdata.h \
	libregf_libfdatetime.h \
	libregf_libfwnt.h \
	libregf_libuna.h \
	libregf_named_key.c libregf_named_key.h \
	libregf_notify.c libregf_notify.h \
	libregf_support.c libregf_support.h \
	libregf_types.h \
	libregf_unused.h \
	libregf_value.c libregf_value.h \
	libregf_value_item.c libregf_value_item.h \
	libregf_value_type.c libregf_value_type.h \
	regf_cell_values.h \
	regf_file_header.h \
	regf_hive_bin.h

libregf_la_LIBADD = \
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
	@LIBFDATETIME_LIBADD@ \
	@LIBFWNT_LIBADD@ \
	@PTHREAD_LIBADD@

libregf_la_LDFLAGS = -no-undefined -version-info 1:0:0

EXTRA_DIST = \
	libregf_definitions.h.in \
	libregf.rc \
	libregf.rc.in

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f libregf_definitions.h
	/bin/rm -f libregf.rc
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libregf ..."
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libregf_la_SOURCES)

