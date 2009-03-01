pkglib_LTLIBRARIES = openvanilla.la
pkglibdir=$(libdir)/ucimf/

openvanilla_la_SOURCES = openvanilla.cpp openvanilla.h debug.h
openvanilla_la_CPPFLAGS = -I/$(includedir) -I. 
openvanilla_la_LDFLAGS = -module -avoid-version -no-undefined -Wl,-E -Bsymbolic -lltdl 

EXTRA_DIST= OpenVanilla/OVLibrary.h OpenVanilla/OVUtility.h OpenVanilla/OpenVanilla.h
