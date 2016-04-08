MSVSCPP_FILES = \
	dokan/dokan.vcproj \
	libbfio/libbfio.vcproj \
	libcaes/libcaes.vcproj \
	libcdata/libcdata.vcproj \
	libcerror/libcerror.vcproj \
	libcfile/libcfile.vcproj \
	libclocale/libclocale.vcproj \
	libcnotify/libcnotify.vcproj \
	libcpath/libcpath.vcproj \
	libcsplit/libcsplit.vcproj \
	libcstring/libcstring.vcproj \
	libcsystem/libcsystem.vcproj \
	libcthreads/libcthreads.vcproj \
	libfcache/libfcache.vcproj \
	libfdata/libfdata.vcproj \
	libqcow/libqcow.vcproj \
	libuna/libuna.vcproj \
	pyqcow/pyqcow.vcproj \
	qcow_test_open_close/qcow_test_open_close.vcproj \
	qcow_test_read/qcow_test_read.vcproj \
	qcow_test_seek/qcow_test_seek.vcproj \
	qcowinfo/qcowinfo.vcproj \
	qcowmount/qcowmount.vcproj \
	zlib/zlib.vcproj \
	libqcow.sln

SCRIPT_FILES = \
	scripts/vs2008_x64.sh \
	scripts/vs2008_x64_sln.sed \
	scripts/vs2008_x64_vcproj.sed \
	scripts/vs2010_x64.sh \
	scripts/vs2010_x64_sln.sed \
	scripts/vs2010_x64_vcxproj.sed

EXTRA_DIST = \
	$(MSVSCPP_FILES) \
	$(SCRIPT_FILES)

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

