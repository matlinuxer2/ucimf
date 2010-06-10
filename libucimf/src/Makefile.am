AM_CPPFLAGS = -g -DDEBUG 

lib_LTLIBRARIES = libucimf.la
libucimf_la_SOURCES = cwm.h shape.h options.h debug.h \
                        options.cpp \
			type.cpp graphport.cpp \
			window.cpp widget.cpp shape.cpp cwm.cpp \
			ucimf.cpp $(top_builddir)/font/libfont.a 


libucimf_la_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include -I$(top_srcdir)/include/imf -I$(top_srcdir)/font  -I$(top_srcdir)/display
libucimf_la_LDFLAGS = -lltdl 
libucimf_la_LIBADD = $(top_builddir)/display/libdisplay.a $(top_builddir)/font/libfont.a 

bin_PROGRAMS = ucimf_keyboard
ucimf_keyboard_SOURCES = ucimf_keyboard.cpp
