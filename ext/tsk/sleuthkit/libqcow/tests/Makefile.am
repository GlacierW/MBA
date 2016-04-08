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
	test_pyqcow_open_close.sh \
	test_pyqcow_seek.sh \
	test_pyqcow_read.sh
endif

TESTS = \
	test_open_close.sh \
	test_seek.sh \
	test_read.sh \
	test_qcowinfo.sh \
	$(TESTS_PYQCOW)

check_SCRIPTS = \
	pyqcow_test_open_close.py \
	pyqcow_test_read.py \
	pyqcow_test_seek.py \
	test_open_close.sh \
	test_pyqcow_open_close.sh \
	test_pyqcow_read.sh \
	test_pyqcow_seek.sh \
	test_qcowinfo.sh \
	test_read.sh \
	test_runner.sh \
	test_seek.sh

EXTRA_DIST = \
	$(check_SCRIPTS)

check_PROGRAMS = \
	qcow_test_open_close \
	qcow_test_read \
	qcow_test_seek

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

