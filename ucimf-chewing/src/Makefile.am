pkglib_LTLIBRARIES = chewing.la
pkglibdir=$(libdir)/ucimf/

chewing_la_SOURCES = chewing.cpp uchewing.h 
chewing_la_CPPFLAGS = -I$(includedir) -I$(top_srcdir)/include $(chewing_CFLAGS) -DCHEWING_DATA_PREFIX="\"/usr/share/chewing/\"" 

chewing_la_LIBADD = $(chewing_LIBS)
chewing_la_LDFLAGS = -module -avoid-version -no-undefined -Wl,-E -Bsymbolic 
