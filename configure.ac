dnl ==============================================================
dnl Process this file with autoconf to produce a configure script.
dnl ==============================================================

AC_PREREQ([2.60])

AC_INIT([tbt],[1.1.0],[jaromil@dyne.org],[tbt])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_HOST

dnl backwards compatibility for autoconf >= 2.64
dnl PACKAGE_URL should be the fifth argument of AC_INIT
m4_define([AC_PACKAGE_URL], [http://tbt.dyne.org])
dnl AC_DEFINE(PACKAGE_URL, "AC_PACKAGE_URL", [Package URL])
AC_SUBST(PACKAGE_URL, AC_PACKAGE_URL)

AC_CANONICAL_HOST
AC_PROG_MAKE_SET
AC_PROG_INSTALL

dnl Setup for automake
dnl ==============================================================
dnl Setup for automake
dnl ==============================================================
AM_INIT_AUTOMAKE([dist-bzip2 subdir-objects -Wno-portability])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_SRCDIR([src/tbt.cpp])

dnl Checks for required programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_RANLIB

dnl Check for slang
PKG_CHECK_MODULES([SLANG], [slang], [have_slang=yes], [have_slang=no])
if test x$have_slang = xno; then
   AC_MSG_ERROR([*** SLANG development files not found!])
fi

dnl Checks for header files.
AC_HEADER_STDC

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST


AC_CONFIG_FILES([
Makefile
src/Makefile
src/slw/Makefile
doc/Makefile
test/Makefile
])
AC_OUTPUT

dnl function to print verbose configure options only if V=1 is passed to
dnl configure
AC_DEFUN([VRB],
         AS_IF([test x"$V" == x1], INFO([$1])))

dnl autoconf < 2.63 compatibility
m4_ifndef([AS_VAR_APPEND],
          AC_DEFUN([AS_VAR_APPEND], $1=$$1$2))

dnl convenience function so that INFO messages go to config.log and to stdout,
dnl useful when debugging user problems only config.log is needed
AC_DEFUN([INFO],
         AS_ECHO(["$1"]) >&AS_MESSAGE_LOG_FD
         AS_ECHO(["$1"]) >&AS_MESSAGE_FD)

dnl as above, but no newline at the end
AC_DEFUN([INFO_N],
         AS_ECHO_N(["$1"]) >&AS_MESSAGE_LOG_FD
         AS_ECHO_N(["$1"]) >&AS_MESSAGE_FD)
