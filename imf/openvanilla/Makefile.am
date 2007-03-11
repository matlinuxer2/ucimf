INCLUDES = -I$(includedir)/imf/ -I$(includedir)/ -I$(includedir)/OpenVanilla/

#OV_MODULEDIR= $(prefix)/lib/openvanilla/

pkglib_LTLIBRARIES = openvanilla.la
pkglibdir=$(libdir)/ucimf/

EXTRA_DIST= OpenVanilla/*

openvanilla_la_SOURCES = openvanilla.cpp openvanilla.h
openvanilla_la_LDFLAGS = -module -avoid-version -no-undefined -Wl,-E -Bsymbolic -lltdl

