#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([fbterm_ucimf], [0.2.9], [ucimf@googlegroups.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([fbterm_ucimf.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

dnl Checking UCIMF.......................................................
PKG_CHECK_MODULES([ucimf], [libucimf])                                                        
AC_SUBST(ucimf_CFLAGS)
AC_SUBST(ucimf_LIBS)
dnl .........................................................................

dnl Checking FreeType2.......................................................
PKG_CHECK_MODULES([freetype2], [freetype2])                                                        
AC_SUBST(freetype2_CFLAGS)
AC_SUBST(freetype2_LIBS)
dnl .........................................................................

dnl Checking Fontconfig.......................................................
PKG_CHECK_MODULES([fontconfig], [fontconfig])
AC_SUBST(fontconfig_CFLAGS)
AC_SUBST(fontconfig_LIBS)
dnl .........................................................................

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

AH_VERBATIM([USE_DEBUG],[#undef USE_DEBUG])
AC_ARG_ENABLE([debug],
        AS_HELP_STRING([--enable-debug],
                [enable Debug (default: no)]),
        debug=$enableval, debug="no")

if test "$debug" = yes; then                                                                     
	AC_DEFINE([USE_DEBUG], [], [])
fi
AM_CONDITIONAL(ENABLE_DEBUG, test "$debug" = "yes")


# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([setenv strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
