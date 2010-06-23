noinst_LTLIBRARIES = libfont.la

libfont_la_SOURCES = font.h font.cpp 
libfont_la_CPPFLAGS = $(freetype2_CFLAGS) $(fontconfig_CFLAGS) -fPIC 
libfont_la_LIBADD = $(freetype2_LIBS) $(fontconfig_LIBS) 


## below is for test programs

noinst_PROGRAMS = font-c font-cpp

font_c_SOURCES = main-c.c 
font_c_LDADD =  libfont.la -lstdc++

font_cpp_SOURCES = main-cpp.cpp
font_cpp_LDADD =  libfont.la 

