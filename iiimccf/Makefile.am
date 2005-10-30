SUBDIRS = display

EXTRA_INCS = -I/usr/include/freetype2 
EXTRA_LIBS = -Ldisplay/ -ldisplay

AM_CPPFLAGS = -g -DDEBUG $(EXTRA_INCS)

lib_LTLIBRARIES = libiiimccf.la

libiiimccf_la_SOURCES = iiimccf.cpp \
                        iiimccf-core.cpp iiimccf-component.cpp iiimccf-util.cpp \
			iiimccf-preedit.cpp iiimccf-lookup_choice.cpp \
                        overspot.cpp layer.cpp font.cpp graphdev.cpp
libiiimccf_la_LDFLAGS = 
libiiimccf_la_LIBADD = -L$(libdir) $(EXTRA_LIBS) -lfreetype -liiimcf 
