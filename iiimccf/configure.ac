#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(iiimccf, 0.8 , BUG-REPORT-ADDRESS)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([overspot.h])
AC_CONFIG_HEADER([config.h])
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-ldisplay':
AC_CHECK_LIB([display], [main])
# FIXME: Replace `main' with a function in `-lfreetype':
AC_CHECK_LIB([freetype], [main])
# FIXME: Replace `main' with a function in `-lgraphdev':
AC_CHECK_LIB([graphdev], [main])
# FIXME: Replace `main' with a function in `-liiimccf':
AC_CHECK_LIB([iiimccf], [main])
# FIXME: Replace `main' with a function in `-liiimcf':
AC_CHECK_LIB([iiimcf], [main])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdint.h string.h sys/ioctl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove memset munmap])
#AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_CONFIG_FILES([Makefile
                 display/Makefile])
AC_OUTPUT
