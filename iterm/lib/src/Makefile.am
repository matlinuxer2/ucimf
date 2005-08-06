SUBDIRS		= unix  .

CPPFLAGS	= -I$(top_srcdir)/include -DBIDI

if DEBUG
AM_CFLAGS= -g -DDEBUG
endif

lib_LTLIBRARIES = libiterm.la

if BIDI
libiterm_la_SOURCES 	= core.c screen.c screen_visual_row.h screen_internal.h state.c vt100.c screen_buffer.c screen_row.c state.h vt100.h screen_buffer.h screen_row.h tabs.c mbchar.h screen_visual_row.c tabs.h
else
libiterm_la_SOURCES 	= core.c screen.c screen_internal.h state.c vt100.c screen_buffer.c screen_row.c state.h vt100.h screen_buffer.h screen_row.h tabs.c mbchar.h tabs.h dummy_layout.c screen_visual_row.c screen_visual_row.h
endif

libiterm_la_LDFLAGS 	= -version-info 1:0:0

if PLS
  libiterm_la_LIBADD	= unix/tty/libttyio.la unix/pls_layout/libpls.la
else
if FRIBIDI
  libiterm_la_LIBADD	= unix/tty/libttyio.la unix/fribidi_layout/libfribidi.la
else
  libiterm_la_LIBADD	= unix/tty/libttyio.la
endif
endif
