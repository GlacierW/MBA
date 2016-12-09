if HAVE_LOCAL_LIBCDATA
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@LIBCERROR_CPPFLAGS@ \
	@LIBCTHREADS_CPPFLAGS@ \
	@PTHREAD_CPPFLAGS@

noinst_LTLIBRARIES = libcdata.la

libcdata_la_SOURCES = \
	libcdata_array.c libcdata_array.h \
	libcdata_btree.c libcdata_btree.h \
	libcdata_definitions.h \
	libcdata_error.c libcdata_error.h \
	libcdata_extern.h \
	libcdata_libcerror.h \
	libcdata_libcthreads.h \
	libcdata_list.c libcdata_list.h \
	libcdata_list_element.c libcdata_list_element.h \
	libcdata_range_list.c libcdata_range_list.h \
	libcdata_range_list_value.c libcdata_range_list_value.h \
	libcdata_support.c libcdata_support.h \
	libcdata_tree_node.c libcdata_tree_node.h \
	libcdata_types.h \
	libcdata_unused.h
endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libcdata ..."
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libcdata_la_SOURCES)

