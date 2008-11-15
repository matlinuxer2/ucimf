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
libucimf_la_SOURCES = options.cpp \
                        graphdev.cpp font.cpp \
			type.cpp graphport.cpp \
			window.cpp widget.cpp shape.cpp cwm.cpp \
			ucimf.cpp

libucimf_la_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include -I$(top_srcdir)/include/imf -DUCIMF_CONF_DIR=\"@UCIMF_CONF_PATH@\"
libucimf_la_LDFLAGS = -lltdl -ldisplay
libucimf_la_LIBADD = -L$(libdir) -L./display/

bin_PROGRAMS = ucimf_keyboard
ucimf_keyboard_SOURCES = ucimf_keyboard.cpp

bin_SCRIPTS = ucimf

confdir=$(sysconfdir)
conf_DATA = ucimf.conf 

miscdir=$(datadir)/ucimf/
misc_DATA = fonts/unifont.pcf ucimf.conf.example

install-exec-hook:
	chown root:utmp $(DESTDIR)$(bindir)/ucimf
	chmod ug+s $(DESTDIR)$(bindir)/ucimf

EXTRA_DIST=  ucimf cwm.h font.h graphdev.h shape.h options.h ucimf.conf ucimf.conf.example fonts/unifont.pcf

