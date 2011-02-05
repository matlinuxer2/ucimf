pkglib_LTLIBRARIES = sunpinyin.la
pkglibdir=$(libdir)/ucimf/

sunpinyin_la_SOURCES = sunpinyin.cpp sunpinyin.h 
sunpinyin_la_CPPFLAGS = -I$(top_srcdir)/include $(sunpinyin_CFLAGS)
sunpinyin_la_LIBADD = $(sunpinyin_LIBS)
sunpinyin_la_LDFLAGS = -module -avoid-version -no-undefined -Wl,-E -Bsymbolic 

