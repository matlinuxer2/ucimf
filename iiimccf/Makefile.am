SUBDIRS = display

EXTRA_INCS =
EXTRA_LIBS = -Ldisplay/ -ldisplay

AM_CPPFLAGS = -g -DDEBUG $(EXTRA_INCS) -I/usr/include/iiim

lib_LTLIBRARIES = libiiimccf.la

iiimccfincludedir = $(includedir)
iiimccfinclude_HEADERS = iiimccf.h

libiiimccf_la_SOURCES = iiimccf.cpp \
                        iiimccf-core.cpp iiimccf-component.cpp iiimccf-util.cpp \
			iiimccf-preedit.cpp iiimccf-lookup_choice.cpp iiimccf-commit.cpp\
                        overspot.cpp layer.cpp font.cpp graphdev.cpp
libiiimccf_la_LDFLAGS = 
libiiimccf_la_LIBADD = \
	-L$(libdir) \
	$(EXTRA_LIBS) \
	-liiimcf 
