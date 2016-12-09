if HAVE_PYTHON
AM_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@LIBCERROR_CPPFLAGS@ \
	@LIBCDATA_CPPFLAGS@ \
	@LIBCLOCALE_CPPFLAGS@ \
	@LIBCSPLIT_CPPFLAGS@ \
	@LIBUNA_CPPFLAGS@ \
	@LIBCFILE_CPPFLAGS@ \
	@LIBCPATH_CPPFLAGS@ \
	@LIBBFIO_CPPFLAGS@

pyexec_LTLIBRARIES = pyregf.la

pyregf_la_SOURCES = \
	pyregf.c pyregf.h \
	pyregf_codepage.c pyregf_codepage.h \
	pyregf_datetime.c pyregf_datetime.h \
	pyregf_error.c pyregf_error.h \
	pyregf_file.c pyregf_file.h \
	pyregf_file_object_io_handle.c pyregf_file_object_io_handle.h \
	pyregf_file_types.c pyregf_file_types.h \
	pyregf_integer.c pyregf_integer.h \
	pyregf_key.c pyregf_key.h \
	pyregf_keys.c pyregf_keys.h \
	pyregf_libbfio.h \
	pyregf_libcerror.h \
	pyregf_libclocale.h \
	pyregf_libregf.h \
	pyregf_python.h \
	pyregf_unused.h \
	pyregf_value.c pyregf_value.h \
	pyregf_value_types.c pyregf_value_types.h \
	pyregf_values.c pyregf_values.h

pyregf_la_LIBADD = \
	@LIBCERROR_LIBADD@ \
	../libregf/libregf.la \
	@LIBBFIO_LIBADD@ \
	@LIBCPATH_LIBADD@ \
	@LIBCFILE_LIBADD@ \
	@LIBCSPLIT_LIBADD@ \
	@LIBCLOCALE_LIBADD@ \
	@LIBCDATA_LIBADD@

pyregf_la_CPPFLAGS = $(PYTHON_CPPFLAGS)
pyregf_la_LDFLAGS  = -module -avoid-version $(PYTHON_LDFLAGS)

endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

