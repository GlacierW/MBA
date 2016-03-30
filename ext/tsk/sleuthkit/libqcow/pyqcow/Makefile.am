if HAVE_PYTHON
AM_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@LIBCSTRING_CPPFLAGS@ \
	@LIBCERROR_CPPFLAGS@ \
	@LIBCDATA_CPPFLAGS@ \
	@LIBCLOCALE_CPPFLAGS@ \
	@LIBCSPLIT_CPPFLAGS@ \
	@LIBUNA_CPPFLAGS@ \
	@LIBCFILE_CPPFLAGS@ \
	@LIBCPATH_CPPFLAGS@ \
	@LIBBFIO_CPPFLAGS@

pyexec_LTLIBRARIES = pyqcow.la

pyqcow_la_SOURCES = \
	pyqcow.c pyqcow.h \
	pyqcow_encryption_types.c pyqcow_encryption_types.h \
	pyqcow_error.c pyqcow_error.h \
	pyqcow_file.c pyqcow_file.h \
	pyqcow_file_object_io_handle.c pyqcow_file_object_io_handle.h \
	pyqcow_integer.c pyqcow_integer.h \
	pyqcow_libbfio.h \
	pyqcow_libcerror.h \
	pyqcow_libclocale.h \
	pyqcow_libcstring.h \
	pyqcow_libqcow.h \
	pyqcow_python.h \
	pyqcow_unused.h

pyqcow_la_LIBADD = \
	@LIBCSTRING_LIBADD@ \
	@LIBCERROR_LIBADD@ \
	../libqcow/libqcow.la \
	@LIBCDATA_LIBADD@ \
	@LIBCLOCALE_LIBADD@ \
	@LIBCSPLIT_LIBADD@ \
	@LIBUNA_LIBADD@ \
	@LIBCFILE_LIBADD@ \
	@LIBCPATH_LIBADD@ \
	@LIBBFIO_LIBADD@

pyqcow_la_CPPFLAGS = $(PYTHON_CPPFLAGS)
pyqcow_la_LDFLAGS  = -module -avoid-version $(PYTHON_LDFLAGS)

endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

