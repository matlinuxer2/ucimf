noinst_PROGRAMS = console01 console04

console01_SOURCES = term/console01.cpp
console01_CPPFLAGS = -I$(top_srcdir)/include
console01_LDADD = $(top_builddir)/src/libucimf.la

console04_SOURCES = term/console04.cpp
console04_CPPFLAGS = -I$(top_srcdir)/include
console04_LDADD = $(top_builddir)/src/libucimf.la

############################

pkglib_LTLIBRARIES = dummy.la
pkglibdir=$(libdir)/ucimf

dummy_la_SOURCES = imf/dummy.cpp imf/dummy.h
dummy_la_CPPFLAGS = -I$(top_srcdir)/include
dummy_la_LIBADD = $(top_builddir)/src/libucimf.la
dummy_la_LDFLAGS = -module -avoid-version -no-undefined
