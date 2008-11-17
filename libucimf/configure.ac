#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(libucimf, 2.1.0, ucimf@googlegroups.com)
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

# Checks for libraries.
# FIXME: Replace `main' with a function in `-ldisplay':
AC_CHECK_LIB([display], [main])
# FIXME: Replace `main' with a function in `-lgraphdev':
AC_CHECK_LIB([graphdev], [main])
# FIXME: Replace `main' with a function in `-lltdl':
AC_CHECK_LIB([ltdl], [main])

dnl Checking FreeType2.......................................................
AC_CHECK_LIB(freetype,
	FT_Init_FreeType, 
	[], 
	[AC_MSG_ERROR([freetype2 is required to build ucimf])])
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

# Check for customized module path
# if test "x$IMF_MODULE_DIR" = "x"; then
#   IMF_MODULE_DIR=${prefix}/lib/ucimf
# fi

#Check for customized conf file path
if test "x$UCIMF_CONF_PATH" = "x"; then
  UCIMF_CONF_PATH="/etc/"
fi

AC_SUBST(UCIMF_CONF_PATH)



# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove memset munmap setlocale strstr])

AC_CONFIG_FILES([Makefile
                 display/Makefile])
AC_OUTPUT
