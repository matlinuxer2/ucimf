SUBDIRS = display
OV_MODULEDIR= /home/mat/usr/lib/openvanilla/

EXTRA_INCS = -I/usr/include/iiim/ -I/usr/include/OpenVanilla/
EXTRA_LIBS = -Ldisplay/ -ldisplay -liiimcf -lltdl

AM_CPPFLAGS = -g -DDEBUG $(EXTRA_INCS) 

lib_LTLIBRARIES = libucimf.la

ucimfincludedir = $(includedir)
ucimfinclude_HEADERS = ucimf.h

libucimf_la_SOURCES = ucimf.cpp \
			iiimccf.cpp openvanilla.cpp \
			widget.cpp layer.cpp\
                        font.cpp graphdev.cpp 
libucimf_la_LDFLAGS = 
libucimf_la_LIBADD = -L$(libdir) $(EXTRA_LIBS) 
