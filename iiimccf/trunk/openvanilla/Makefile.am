INCLUDES = -I. -I.. -I$(prefix)/include/OpenVanilla/

OV_MODULEDIR= $(prefix)/lib/openvanilla/

pkglib_LTLIBRARIES = openvanilla.la

EXTRA_DIST = openvanilla.h
openvanilla_la_SOURCES = openvanilla.cpp
openvanilla_la_LDFLAGS = -module -avoid-version -no-undefined -Wl,-E -Bsymbolic
openvanilla_la_LIBADD = -lltdl


