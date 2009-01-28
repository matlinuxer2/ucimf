pkglib_LTLIBRARIES = openvanilla.la
pkglibdir=$(libdir)/ucimf/

openvanilla_la_SOURCES = openvanilla.cpp openvanilla.h
openvanilla_la_CPPFLAGS = -I/$(includedir) -I. -DOV_MODULEDIR=\"@OV_MODULEDIR@\"
openvanilla_la_LDFLAGS = -module -avoid-version -no-undefined -Wl,-E -Bsymbolic -lltdl 

EXTRA_DIST= OpenVanilla/OVLibrary.h OpenVanilla/OVUtility.h OpenVanilla/OpenVanilla.h
