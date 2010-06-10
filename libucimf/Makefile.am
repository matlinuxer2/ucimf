SUBDIRS = display font src 

AM_CPPFLAGS = -g -DDEBUG 

ucimfinclude_HEADERS = include/ucimf.h
ucimfincludedir = $(includedir)

imfinclude_HEADERS = include/imf/graphport.h include/imf/imf.h include/imf/type.h include/imf/widget.h include/imf/window.h
imfincludedir = $(includedir)/imf/

bin_SCRIPTS = ucimf_start ucimf_keyboard

confdir=$(sysconfdir)
conf_DATA = ucimf.conf 

miscdir=$(datadir)/ucimf/
misc_DATA = ucimf.conf.example

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libucimf.pc libucimf-uninstalled.pc
pkgconfig_in_data = libucimf.pc.in libucimf-uninstalled.pc.in

EXTRA_DIST=  $(bin_SCRIPTS) $(conf_DATA) $(misc_DATA) $(pkgconfig_DATA) $(pkgconfig_in_data)


