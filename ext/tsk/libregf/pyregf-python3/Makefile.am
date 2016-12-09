# Note that we cannot use: AUTOMAKE_OPTIONS = subdir-objects
# subdir-objects will compile the source files to a single version of Python.
# Since subdir-objects is being deprecated we copy the source files instead.
am__installdirs = "$(DESTDIR)$(pyexecdir3)"
pyexecdir = $(pyexecdir3)

if HAVE_PYTHON3
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

am_pyregf_la_rpath = -rpath $(pyexecdir3)

BUILT_SOURCES = \
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

pyexec_LTLIBRARIES = pyregf.la

nodist_pyregf_la_SOURCES = $(BUILT_SOURCES)

pyregf_la_LIBADD = \
	@LIBCERROR_LIBADD@ \
	../libregf/libregf.la \
	@LIBBFIO_LIBADD@ \
	@LIBCPATH_LIBADD@ \
	@LIBCFILE_LIBADD@ \
	@LIBCSPLIT_LIBADD@ \
	@LIBCLOCALE_LIBADD@ \
	@LIBCDATA_LIBADD@

pyregf_la_CPPFLAGS = $(PYTHON3_CPPFLAGS)
pyregf_la_LDFLAGS  = -module -avoid-version $(PYTHON3_LDFLAGS)

$(BUILT_SOURCES):
	/bin/cp -f $(top_srcdir)/pyregf/$@ $@

endif

MAINTAINERCLEANFILES = \
	Makefile.in

clean-local:
	/bin/rm -f pyregf*.[ch]

distclean: clean
	/bin/rm -f Makefile

