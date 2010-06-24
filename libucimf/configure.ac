#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(libucimf, 2.3.5, ucimf@googlegroups.com)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AM_PROG_CC_C_O

# Checks for libraries.
AC_CHECK_LIB([ltdl], 
	[lt_dlopen],
	[], 
	[AC_MSG_ERROR([ltdl is required to build libucimf])])

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
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h fcntl.h locale.h stdlib.h string.h sys/ioctl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

test "x$prefix" = xNONE && prefix=$ac_default_prefix
test "x$exec_prefix" = xNONE && exec_prefix=${prefix}
SYSCONFDIR="`eval echo $sysconfdir`"
LIBDIR="`eval echo $libdir`"
#DATADIR="`eval echo $datadir`"
AC_DEFINE_UNQUOTED([SYSCONFDIR], ["${SYSCONFDIR}"], [Description])
AC_DEFINE_UNQUOTED([LIBDIR], ["${LIBDIR}"], [Description])
#AC_DEFINE_UNQUOTED([DATADIR], ["${DATADIR}"], [Description])

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
AC_FUNC_CLOSEDIR_VOID
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove memset munmap setlocale strstr])

AC_CONFIG_FILES([Makefile
		 libucimf-uninstalled.pc
		 libucimf.pc
                 src/Makefile
                 font/Makefile
                 display/Makefile])

AC_OUTPUT
