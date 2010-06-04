SUBDIRS = display font

AM_CPPFLAGS = -g -DDEBUG 

ucimfinclude_HEADERS = include/ucimf.h
ucimfincludedir = $(includedir)

imfinclude_HEADERS = include/imf/graphport.h include/imf/imf.h include/imf/type.h include/imf/widget.h include/imf/window.h
imfincludedir = $(includedir)/imf/

lib_LTLIBRARIES = libucimf.la
libucimf_la_SOURCES = options.cpp \
                        graphdev.cpp \
			type.cpp graphport.cpp \
			window.cpp widget.cpp shape.cpp cwm.cpp \
			ucimf.cpp $(top_builddir)/font/libfont.a 


libucimf_la_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include -I$(top_srcdir)/include/imf -I$(top_srcdir)/font 
libucimf_la_LDFLAGS = -lltdl 
libucimf_la_LIBADD = $(top_builddir)/display/libdisplay.a $(top_builddir)/font/libfont.a 

bin_PROGRAMS = ucimf_keyboard
ucimf_keyboard_SOURCES = ucimf_keyboard.cpp

bin_SCRIPTS = ucimf_start

confdir=$(sysconfdir)
conf_DATA = ucimf.conf 

miscdir=$(datadir)/ucimf/
misc_DATA = ucimf.conf.example

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libucimf.pc libucimf-uninstalled.pc
pkgconfig_in_data = libucimf.pc.in libucimf-uninstalled.pc.in

install-exec-hook:
	chown root:utmp $(DESTDIR)$(bindir)/ucimf_start || echo "Change ownership failed..."
	chmod ug+s $(DESTDIR)$(bindir)/ucimf_start

EXTRA_DIST=  $(bin_SCRIPTS) cwm.h graphdev.h shape.h options.h $(conf_DATA) $(misc_DATA) $(pkgconfig_DATA) $(pkgconfig_in_data)


