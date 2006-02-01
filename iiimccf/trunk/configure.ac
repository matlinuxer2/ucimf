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

dnl Checking FreeType2.......................................................
AC_CHECK_PROG(FREETYPE_CONFIG, freetype-config, yes, no)
if test $FREETYPE_CONFIG = yes; then
  # These are in fact CPPFLAGS
  CPPFLAGS="$CPPFLAGS `freetype-config --cflags`"
  # This is a mix of LDFLAGS and LIBS...
  FT_BORDEL=`freetype-config --libs`
  FT_LIBS=""
  FT_LDFLAGS=""
  for word in $FT_BORDEL ; do
    beginning=`echo $word | cut -c -2`
    if test ".$beginning" = ".-L"; then
      FT_LDFLAGS="$FT_LDFLAGS $word"
    else
      FT_LIBS="$FT_LIBS $word"
    fi
  done
  LDFLAGS="$LDFLAGS $FT_LDFLAGS"
  LIBS="$LIBS -lfreetype $FT_LIBS"
else
    AC_MSG_ERROR([freetype2 is required to build iiimccf])
fi
# always fail on some recent linux distribution ?
AC_CHECK_LIB(freetype,
	FT_Init_FreeType, 
	[], 
	[AC_MSG_ERROR([freetype2 is required to build iiimccf])])
dnl .........................................................................

dnl Checking for libxml++
PKG_CHECK_MODULES([XMLPP], [libxml++-2.6])
AC_SUBST([XMLPP_LIBS])
AC_SUBST([XMLPP_CFLAGS])

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

AC_CONFIG_FILES([
	Makefile
	display/Makefile
])
AC_OUTPUT
