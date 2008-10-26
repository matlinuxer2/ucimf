INCLUDES = -I/$(includedir) -I. -I../libucimf/include -DOV_MODULEDIR=\"@OV_MODULEDIR@\"
pkglib_LTLIBRARIES = openvanilla.la
pkglibdir=$(libdir)/ucimf/

openvanilla_la_SOURCES = openvanilla.cpp openvanilla.h
openvanilla_la_LDFLAGS = -module -avoid-version -no-undefined -Wl,-E -Bsymbolic -lltdl 

