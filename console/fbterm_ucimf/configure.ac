#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([fbterm_ucimf], [0.2.0], [ucimf@googlegroups.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([fbterm_ucimf.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-limclient':
AC_CHECK_LIB([ucimf], [ucimf_init])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([setenv strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
