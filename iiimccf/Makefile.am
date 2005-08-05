SUBDIRS = display

EXTRA_INCS = -I/usr/include/freetype2
EXTRA_LIBS = -Ldisplay/

AM_CPPFLAGS = -g -DDEBUG $(EXTRA_INCS)

bin_PROGRAMS = testing
testing_SOURCES = console-app.c
testing_LIBADD = -L. -liiimccf.la

lib_LTLIBRARIES = libiiimccf.la

libiiimccf_la_SOURCES = iiimccf.cpp iiimccf-core.cpp iiimccf-component iiimccf-utils overspot.cpp layer.cpp font.cpp graphdev.cpp
libiiimccf_la_LDFLAGS = 
libiiimccf_la_LIBADD = -L$(libdir) $(EXTRA_LIBS) -ldisplay -lfreetype -liiimcf 
