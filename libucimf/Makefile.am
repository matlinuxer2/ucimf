SUBDIRS = display 

AM_CPPFLAGS = -g -DDEBUG 

ucimfinclude_HEADERS = include/ucimf.h
ucimfincludedir = $(includedir)

imfinclude_HEADERS = include/imf/graphport.h include/imf/imf.h include/imf/type.h include/imf/widget.h include/imf/window.h
imfincludedir = $(includedir)/imf/

lib_LTLIBRARIES = libucimf.la
libucimf_la_SOURCES = options.cpp \
                        graphdev.cpp font.cpp \
			type.cpp graphport.cpp \
			window.cpp widget.cpp shape.cpp cwm.cpp \
			ucimf.cpp

libucimf_la_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include -I$(top_srcdir)/include/imf `pkg-config --cflags freetype2`
libucimf_la_LDFLAGS = -lltdl `pkg-config --libs freetype2`
libucimf_la_LIBADD = $(top_builddir)/display/libdisplay.a

bin_PROGRAMS = ucimf_keyboard
ucimf_keyboard_SOURCES = ucimf_keyboard.cpp

bin_SCRIPTS = ucimf_start

confdir=$(sysconfdir)
conf_DATA = ucimf.conf 

miscdir=$(datadir)/ucimf/
misc_DATA = fonts/unifont.pcf ucimf.conf.example

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libucimf.pc libucimf-uninstalled.pc

pkgconfig_in_data = libucimf.pc.in libucimf-uninstalled.pc.in

install-exec-hook:
	chown root:utmp $(DESTDIR)$(bindir)/ucimf_start || echo "Change ownership failed..."
	chmod ug+s $(DESTDIR)$(bindir)/ucimf_start

EXTRA_DIST=  ucimf_start cwm.h font.h graphdev.h shape.h options.h ucimf.conf ucimf.conf.example fonts/unifont.pcf debug.h $(pkgconfig_DATA) $(pkgconfig_in_data)


