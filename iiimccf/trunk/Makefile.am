SUBDIRS = display 
OV_MODULEDIR= $(prefix)/lib/openvanilla/

EXTRA_INCS = -I$(prefix)/include/iiim/ -I$(prefix)/include/OpenVanilla/
#EXTRA_LIBS = -Ldisplay/ -ldisplay -liiimcf -lltdl
EXTRA_LIBS = -liiimcf -lltdl -Ldisplay/ -ldisplay 

AM_CPPFLAGS = -g -DDEBUG $(EXTRA_INCS) 

lib_LTLIBRARIES = libucimf.la

ucimfincludedir = $(includedir)
ucimfinclude_HEADERS = ucimf.h

libucimf_la_SOURCES = \
                        graphdev.cpp font.cpp\
			type.cpp graphport.cpp\
			window.cpp widget.cpp shape.cpp cwm.cpp\
			iiimccf.cpp openvanilla.cpp\
			imf.cpp ucimf.cpp

libucimf_la_LDFLAGS = 
libucimf_la_LIBADD = -L$(libdir) $(EXTRA_LIBS) 

