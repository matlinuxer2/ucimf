noinst_LIBRARIES = libfont.a
noinst_PROGRAMS = font-c font-cpp

libfont_a_SOURCES = font.h font.cpp 
libfont_a_CPPFLAGS = $(freetype2_CFLAGS) $(fontconfig_CFLAGS) -static -fPIC

font_c_SOURCES = main-c.c 
font_c_LDADD =  $(freetype2_LIBS) $(fontconfig_LIBS) 
font_c_LDFLAGS = -lfont -L. -lstdc++

font_cpp_SOURCES = main-cpp.cpp
font_cpp_LDADD =  $(freetype2_LIBS) $(fontconfig_LIBS) 
font_cpp_LDFLAGS = -lfont -L. 

