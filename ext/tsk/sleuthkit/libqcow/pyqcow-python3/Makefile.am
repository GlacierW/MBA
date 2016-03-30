# Note that we cannot use: AUTOMAKE_OPTIONS = subdir-objects
# subdir-objects will compile the source files to a single version of Python.
# Since subdir-objects is being deprecated we copy the source files instead.
am__installdirs = "$(DESTDIR)$(pyexecdir3)"
pyexecdir = $(pyexecdir3)

if HAVE_PYTHON3
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

am_pyqcow_la_rpath = -rpath $(pyexecdir3)

BUILT_SOURCES = \
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

pyexec_LTLIBRARIES = pyqcow.la

nodist_pyqcow_la_SOURCES = $(BUILT_SOURCES)

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

pyqcow_la_CPPFLAGS = $(PYTHON3_CPPFLAGS)
pyqcow_la_LDFLAGS  = -module -avoid-version $(PYTHON3_LDFLAGS)

$(BUILT_SOURCES):
	/bin/cp -f $(top_srcdir)/pyqcow/$@ $@

endif

MAINTAINERCLEANFILES = \
	Makefile.in

clean-local:
	/bin/rm -f pyqcow*.[ch]

distclean: clean
	/bin/rm -f Makefile

