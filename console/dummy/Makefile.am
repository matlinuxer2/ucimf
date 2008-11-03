AUTOMAKE_OPTIONS= foreign

AM_CPPFLAGS = -g -DDEBUG

bin_PROGRAMS = console01 console02
console01_SOURCES = console01.cpp
console01_LDFLAGS = -lucimf

console02_SOURCES = console02.cpp
console02_LDFLAGS = -lfont01
console02_CPPFLAGS = `pkg-config --cflags freetype2`

lib_LTLIBRARIES = libfont01.la
libfont01_la_SOURCES = font01.cpp
libfont01_la_LDFLAGS =  `pkg-config --libs freetype2` -lltdl
libfont01_la_CPPFLAGS = `pkg-config --cflags freetype2` -g -DDEBUG

