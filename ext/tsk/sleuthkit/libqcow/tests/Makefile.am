AM_CPPFLAGS = \
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
	@LIBCSYSTEM_CPPFLAGS@ \
	@PTHREAD_CPPFLAGS@ \
	@LIBQCOW_DLL_IMPORT@

if HAVE_PYTHON_TESTS
TESTS_PYQCOW = \
	test_python_functions.sh
endif

TESTS = \
	test_api_functions.sh \
	test_qcowinfo.sh \
	$(TESTS_PYQCOW)

check_SCRIPTS = \
	pyqcow_test_get_version.py \
	pyqcow_test_open_close.py \
	pyqcow_test_read.py \
	pyqcow_test_seek.py \
	test_api_functions.sh \
	test_python_functions.sh \
	test_qcowinfo.sh \
	test_runner.sh

EXTRA_DIST = \
	$(check_SCRIPTS)

check_PROGRAMS = \
	qcow_test_file_get_values \
	qcow_test_get_version \
	qcow_test_open_close \
	qcow_test_read \
	qcow_test_seek

qcow_test_file_get_values_SOURCES = \
	qcow_test_file_get_values.c \
	qcow_test_libcerror.h \
	qcow_test_libcstring.h \
	qcow_test_libqcow.h \
	qcow_test_unused.h

qcow_test_file_get_values_LDADD = \
	@LIBCSYSTEM_LIBADD@ \
	../libqcow/libqcow.la \
	@LIBCERROR_LIBADD@ \
	@LIBCSTRING_LIBADD@

qcow_test_get_version_SOURCES = \
	qcow_test_get_version.c \
	qcow_test_libcstring.h \
	qcow_test_libqcow.h \
	qcow_test_unused.h

qcow_test_get_version_LDADD = \
	../libqcow/libqcow.la \
	@LIBCSTRING_LIBADD@

qcow_test_open_close_SOURCES = \
	qcow_test_libcerror.h \
	qcow_test_libcstring.h \
	qcow_test_libcsystem.h \
	qcow_test_libcthreads.h \
	qcow_test_libqcow.h \
	qcow_test_open_close.c \
	qcow_test_unused.h

qcow_test_open_close_LDADD = \
	@LIBCSYSTEM_LIBADD@ \
	../libqcow/libqcow.la \
	@LIBCTHREADS_LIBADD@ \
	@LIBCERROR_LIBADD@ \
	@LIBCSTRING_LIBADD@ \
	@PTHREAD_LIBADD@

qcow_test_read_SOURCES = \
	qcow_test_libcerror.h \
	qcow_test_libcstring.h \
	qcow_test_libcsystem.h \
	qcow_test_libcthreads.h \
	qcow_test_libqcow.h \
	qcow_test_read.c \
	qcow_test_unused.h

qcow_test_read_LDADD = \
	@LIBCSYSTEM_LIBADD@ \
	../libqcow/libqcow.la \
	@LIBCTHREADS_LIBADD@ \
	@LIBCERROR_LIBADD@ \
	@LIBCSTRING_LIBADD@ \
	@PTHREAD_LIBADD@

qcow_test_seek_SOURCES = \
	qcow_test_libcerror.h \
	qcow_test_libcstring.h \
	qcow_test_libcsystem.h \
	qcow_test_libqcow.h \
	qcow_test_seek.c \
	qcow_test_unused.h

qcow_test_seek_LDADD = \
	@LIBCSYSTEM_LIBADD@ \
	../libqcow/libqcow.la \
	@LIBCERROR_LIBADD@ \
	@LIBCSTRING_LIBADD@ \
	@PTHREAD_LIBADD@

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

