# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(ucimf-dummy,0.0.1 , BUG-REPORT-ADDRESS)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([dummy.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lltdl':
AC_CHECK_LIB([ltdl], [main])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
