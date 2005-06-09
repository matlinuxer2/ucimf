bin_PROGRAMS	= xiterm

EXTRA_LIBS     = -L../../../lib/src/.libs -L../lib/.libs
EXTRA_INCLUDES = -I../lib -I../../../lib/include

xiterm_LDADD   = -L$(libdir) @X_LIBS@ $(EXTRA_LIBS) -lX11 -lXmu -lXt -lXaw -literm -lXiterm
xiterm_SOURCES	= popup_menu.c popup_menu.h  xiterm.c

CPPFLAGS        = -I$(includedir) $(EXTRA_INCLUDES)
