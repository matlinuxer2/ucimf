SUBDIRS = ./display 

AM_CPPFLAGS = -g -DDEBUG 

ucimfinclude_HEADERS = \
			include/imf \
			include/ucimf.h
ucimfincludedir = $(includedir)

imfinclude_HEADERS = \
			include/imf/graphport.h \
			include/imf/imf.h \
			include/imf/type.h \
			include/imf/widget.h \
			include/imf/window.h
imfincludedir = $(includedir)/imf/

lib_LTLIBRARIES = libucimf.la
libucimf_la_SOURCES = display/fbdev.cpp display/fblinear15.cpp display/fblinear16.cpp display/fblinear24.cpp display/fblinear32.cpp display/fblinear4.cpp display/fblinear8.cpp display/fbvgaplanes.cpp display/libggi.cpp display/vgadev.cpp display/lrmi.c \
			display/fbdev.h display/fblinear15.h display/fblinear16.h display/fblinear24.h display/fblinear32.h display/fblinear4.h display/fblinear8.h display/fbvgaplanes.h display/libggi.h display/lrmi.h display/vgadev.h \
			options.cpp \
                        graphdev.cpp font.cpp \
			type.cpp graphport.cpp \
			window.cpp widget.cpp shape.cpp cwm.cpp \
			ucimf.cpp

libucimf_la_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include -I$(top_srcdir)/include/imf -DUCIMF_CONF_DIR=\"@UCIMF_CONF_PATH@\" `pkg-config --cflags freetype2`
libucimf_la_LDFLAGS = -lltdl `pkg-config --libs freetype2`

bin_PROGRAMS = ucimf_keyboard
ucimf_keyboard_SOURCES = ucimf_keyboard.cpp

bin_SCRIPTS = ucimf

confdir=$(sysconfdir)
conf_DATA = ucimf.conf 

miscdir=$(datadir)/ucimf/
misc_DATA = fonts/unifont.pcf ucimf.conf.example

install-exec-hook:
	chown root:utmp $(DESTDIR)$(bindir)/ucimf || echo "Change ownership failed..."
	chmod ug+s $(DESTDIR)$(bindir)/ucimf

EXTRA_DIST=  ucimf cwm.h font.h graphdev.h shape.h options.h ucimf.conf ucimf.conf.example fonts/unifont.pcf

