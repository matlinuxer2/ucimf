AM_CPPFLAGS = -g -DDEBUG 

lib_LTLIBRARIES = libucimf.la
libucimf_la_SOURCES = cwm.h shape.h options.h \
                        options.cpp \
			type.cpp graphport.cpp \
			window.cpp widget.cpp shape.cpp cwm.cpp \
			ucimf.cpp 

libucimf_la_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/include/imf -I$(top_srcdir)/font  -I$(top_srcdir)/display
libucimf_la_LIBADD = ../display/libdisplay.la ../font/libfont.la -lltdl
