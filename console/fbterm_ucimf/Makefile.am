bin_PROGRAMS = fbterm_ucimf 

fbterm_ucimf_SOURCES = immessage.h imapi.h imapi.c keycode.h keycode.c fbterm_ucimf.cpp \
	instance.h font.h font.cpp screen.h screen.cpp debug.h
EXTRA_fbterm_ucimf_SOURCES = screen_render.cpp

fbterm_ucimf_CPPFLAGS = -I$(includedir) 
fbterm_ucimf_CXXFLAGS = -fno-exceptions -fno-rtti `pkg-config --cflags freetype2`
fbterm_ucimf_LDFLAGS = `pkg-config --libs freetype2` -lfontconfig -lz  -L$(libdir) -lucimf 

EXTRA_DIST = inputmethod.txt
