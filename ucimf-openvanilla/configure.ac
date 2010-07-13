#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(ucimf-openvanilla,2.10.11, ucimf@googlegroups.com)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/openvanilla.cpp])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lltdl':
AC_CHECK_LIB([ltdl], [main])

# Checks for header files.
AC_HEADER_DIRENT
AC_CHECK_HEADERS([locale.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_HEADER_STDC
AC_CHECK_FUNCS([memset setlocale strstr])

AC_DEFINE_DIR([LIBDIR], [libdir], [Description])
AC_DEFINE_DIR([DATADIR], [datadir], [Description])

AH_VERBATIM([USE_DEBUG],[#undef USE_DEBUG])
AC_ARG_ENABLE([debug],
        AS_HELP_STRING([--enable-debug],
                [enable Debug (default: no)]),
        debug=$enableval, debug="no")

if test "$debug" = yes; then                                                                     
	AC_DEFINE([USE_DEBUG], [], [])
fi
AM_CONDITIONAL(ENABLE_DEBUG, test "$debug" = "yes")


AC_CONFIG_FILES([Makefile
src/Makefile
])
AC_OUTPUT
