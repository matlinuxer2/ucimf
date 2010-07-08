noinst_PROGRAMS = console01 console04

console01_SOURCES = term/console01.cpp
console01_CPPFLAGS = -I../include
console01_LDADD = -lucimf -L$(top_builddir)/src

console04_SOURCES = term/console04.cpp
console04_CPPFLAGS = -I../include
console04_LDADD = -lucimf -L$(top_builddir)/src

############################

pkglib_LTLIBRARIES = dummy.la
pkglibdir=$(libdir)/ucimf

dummy_la_SOURCES = imf/dummy.cpp imf/dummy.h
dummy_la_CPPFLAGS = -I../include
dummy_la_LIBADD = -lucimf -L$(top_builddir)/src 
dummy_la_LDFLAGS = -module -avoid-version -no-undefined
