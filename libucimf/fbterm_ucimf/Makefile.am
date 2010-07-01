bin_PROGRAMS = fbterm_ucimf 

fbterm_ucimf_SOURCES = immessage.h imapi.h imapi.c fbterm_ucimf.cpp 
fbterm_ucimf_CPPFLAGS = -I$(top_srcdir)/include/ 
fbterm_ucimf_LDADD = -lucimf 
fbterm_ucimf_LDFLAGS = -L$(top_builddir)/src 

man1_MANS = man/fbterm_ucimf.1

EXTRA= AUTHORS ChangeLog $(man1_MANS)
