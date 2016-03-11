include_HEADERS = \
	libqcow.h

pkginclude_HEADERS = \
	libqcow/codepage.h \
	libqcow/definitions.h \
	libqcow/error.h \
	libqcow/extern.h \
	libqcow/features.h \
	libqcow/types.h

EXTRA_DIST = \
	libqcow.h.in \
	libqcow/definitions.h.in \
	libqcow/features.h.in \
	libqcow/types.h.in

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f libqcow.h
	/bin/rm -f libqcow/definitions.h
	/bin/rm -f libqcow/features.h
	/bin/rm -f libqcow/types.h
	/bin/rm -f Makefile

