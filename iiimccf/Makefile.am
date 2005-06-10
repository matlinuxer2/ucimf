CPPFLAGS	=  

if DEBUG
AM_CFLAGS= -g -DDEBUG
endif

lib_LTLIBRARIES = libiiimccf.la

libiiimccf_la_SOURCES	= iiimccf.c
libiiimccf_la_LDFLAGS 	= -version-info 1:0:0
libiiimccf_la_LIBADD	= unix/tty/libttyio.la
