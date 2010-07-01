pkglib_LTLIBRARIES = openvanilla.la
pkglibdir=$(libdir)/ucimf/

openvanilla_la_SOURCES = openvanilla.cpp ovimf.cpp openvanilla.h debug.h 
openvanilla_la_CPPFLAGS = -I$(top_srcdir)/include
openvanilla_la_LIBADD = -lltdl
openvanilla_la_LDFLAGS = -module -avoid-version -no-undefined -Wl,-E -Bsymbolic 

