AC_INIT([divetools],[1.0.0], [andreas@sandberg.pp.se])
AC_PREREQ(2.59)
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

AC_PROG_CC_STDC

AC_C_BIGENDIAN

AC_CHECK_HEADER([libdivecomputer/device.h], [true], [
  AC_MSG_ERROR([libdivecomputer headers can't be found.])])

AC_CHECK_LIB([divecomputer], [device_foreach], [true], [
  AC_MSG_ERROR([Failed to link agains libdivecomputer.])])

AX_BOOST_BASE([1.40.0], [true],[false], [
  AC_MSG_ERROR([No suitable Boost version found])])
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_FILESYSTEM

AC_ARG_ENABLE([strict],
  AS_HELP_STRING([--disable-strict],
    [Disable strict compile time checks.]),
  [
    true
  ], [
    CFLAGS="-Werror ${CFLAGS}"
  ])

AC_CONFIG_HEADERS([config.h])

CFLAGS="-I$$(top_srcdir)/include $CFLAGS"

AC_CONFIG_FILES([
	Makefile

	include/Makefile
	include/dcxx/Makefile

	lib/Makefile
	lib/dcxx/Makefile

	tools/Makefile
])

AC_OUTPUT
