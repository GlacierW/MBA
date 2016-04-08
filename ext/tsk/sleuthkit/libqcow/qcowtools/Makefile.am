AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@LIBCSTRING_CPPFLAGS@ \
	@LIBCERROR_CPPFLAGS@ \
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
	@LIBCSYSTEM_CPPFLAGS@ \
	@LIBFUSE_CPPFLAGS@ \
	@LIBQCOW_DLL_IMPORT@

AM_LDFLAGS = @STATIC_LDFLAGS@

bin_PROGRAMS = \
	qcowinfo \
	qcowmount

qcowinfo_SOURCES = \
	info_handle.c info_handle.h \
	qcowinfo.c \
	qcowoutput.c qcowoutput.h \
	qcowtools_libbfio.h \
	qcowtools_libcdata.h \
	qcowtools_libcerror.h \
	qcowtools_libclocale.h \
	qcowtools_libcnotify.h \
	qcowtools_libcstring.h \
	qcowtools_libcsystem.h \
	qcowtools_libqcow.h \
	qcowtools_libuna.h

qcowinfo_LDADD = \
	@LIBCSYSTEM_LIBADD@ \
	../libqcow/libqcow.la \
	@LIBCNOTIFY_LIBADD@ \
	@LIBCLOCALE_LIBADD@ \
	@LIBCERROR_LIBADD@ \
	@LIBCSTRING_LIBADD@ \
	@LIBINTL@

qcowmount_SOURCES = \
	mount_handle.c mount_handle.h \
	qcowmount.c \
	qcowoutput.c qcowoutput.h \
	qcowtools_libbfio.h \
	qcowtools_libcdata.h \
	qcowtools_libcerror.h \
	qcowtools_libclocale.h \
	qcowtools_libcnotify.h \
	qcowtools_libcpath.h \
	qcowtools_libcstring.h \
	qcowtools_libcsystem.h \
	qcowtools_libqcow.h \
	qcowtools_libuna.h

qcowmount_LDADD = \
	@LIBFUSE_LIBADD@ \
	@LIBCSYSTEM_LIBADD@ \
	@LIBUNA_LIBADD@ \
	../libqcow/libqcow.la \
	@LIBCNOTIFY_LIBADD@ \
	@LIBCLOCALE_LIBADD@ \
	@LIBCDATA_LIBADD@ \
	@LIBCERROR_LIBADD@ \
	@LIBCSTRING_LIBADD@ \
	@LIBINTL@

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on qcowinfo ..."
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(qcowinfo_SOURCES)
	@echo "Running splint on qcowmount ..."
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(qcowmount_SOURCES)

