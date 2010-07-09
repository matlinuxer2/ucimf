bin_PROGRAMS = fbterm_ucimf 

fbterm_ucimf_SOURCES = immessage.h imapi.h imapi.c fbterm_ucimf.cpp 
fbterm_ucimf_CPPFLAGS = $(ucimf_CFLAGS)
fbterm_ucimf_LDADD = $(ucimf_LIBS)

man1_MANS = man/fbterm_ucimf.1

EXTRA_DIST = AUTHORS ChangeLog $(man1_MANS)
