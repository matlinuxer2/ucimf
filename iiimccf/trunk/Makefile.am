SUBDIRS = display

EXTRA_INCS =
EXTRA_LIBS = -Ldisplay/ -ldisplay

AM_CPPFLAGS = -g -DDEBUG $(EXTRA_INCS) $(XMLPP_CFLAGS) -I/usr/include/iiim

lib_LTLIBRARIES = libiiimccf.la

iiimccfincludedir = $(includedir)
iiimccfinclude_HEADERS = iiimccf.h

libiiimccf_la_SOURCES = iiimccf.cpp observer.cpp \
                        iiimccf-core.cpp iiimccf-util.cpp \
			preedit.cpp lookupchoice.cpp \
                        layer.cpp font.cpp graphdev.cpp 
libiiimccf_la_LDFLAGS = 
libiiimccf_la_LIBADD = \
	-L$(libdir) \
	$(EXTRA_LIBS) \
	$(XMLPP_LIBS) \
	-liiimcf 
