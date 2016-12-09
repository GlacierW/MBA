include_HEADERS = \
	libregf.h

pkginclude_HEADERS = \
	libregf/codepage.h \
	libregf/definitions.h \
	libregf/error.h \
	libregf/extern.h \
	libregf/features.h \
	libregf/types.h

EXTRA_DIST = \
	libregf.h.in \
	libregf/definitions.h.in \
	libregf/features.h.in \
	libregf/types.h.in

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f libregf.h
	/bin/rm -f libregf/definitions.h
	/bin/rm -f libregf/features.h
	/bin/rm -f libregf/types.h
	/bin/rm -f Makefile

