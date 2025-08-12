#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(oemcallapp, 0.1, tridung.nguyen@lge.com)
AC_CONFIG_SRCDIR([src/oem_call_app.cpp])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall gnu foreign subdir-objects])

# Detect CAN type
AC_ARG_WITH([cantype],
    AS_HELP_STRING([--with-cantype],[select cantype or not]),
    [with_cantype=$withval],
    [with_cantype=no])

# Flags of CAN_TYPE
if test "x$with_cantype" == "xgen3"; then
  AM_CPPFLAGS="$AM_CPPFLAGS -DCAN_TYPE_GEN3"
elif test "x$with_cantype" == "xgen2p"; then
  AM_CPPFLAGS="$AM_CPPFLAGS -DCAN_TYPE_GEN2P"
elif test "x$with_cantype" == "xgen2"; then
  AM_CPPFLAGS="$AM_CPPFLAGS -DCAN_TYPE_GEN2"
elif test "x$with_cantype" == "xgen1"; then
  AM_CPPFLAGS="$AM_CPPFLAGS -DCAN_TYPE_GEN1"
else
  AM_CPPFLAGS="$AM_CPPFLAGS -DCAN_TYPE_DEFAULT"
fi

# Detect Region
AC_ARG_WITH(region,
    AC_HELP_STRING([--with-region],[select region or not]),
    [with_region=$withval],
    [with_region=no])

# FLAGS section of REGION_PCB
if test "x$with_region" == "xpcb26"; then
  AM_CPPFLAGS="$AM_CPPFLAGS -DREGION_PCB26"
  IDS_LIBS="-liidsibinder"
  VEHICLE_INFOR_LIB="-lvehiclevariationinfo"
else
  IDS_LIBS=""
  VEHICLE_INFOR_LIB=""
fi

# Flags used when condition processing is required in Makefile.am (optional)
AM_CONDITIONAL(CAN_TYPE_GEN2P, test "x${with_cantype}" = "xgen2p")
AM_CONDITIONAL(CAN_TYPE_GEN3, test "x${with_cantype}" = "xgen3")
AM_CONDITIONAL(CAN_TYPE_GEN2, test "x${with_cantype}" = "xgen2")
AM_CONDITIONAL(CAN_TYPE_GEN1, test "x${with_cantype}" = "xgen1")
AM_CONDITIONAL(REGION_PCB26, test "x${with_region}" = "xpcb26")

AM_PROG_AR
LT_INIT([dlopen])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_LIBTOOL_DLOPEN

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_SUBST(AM_CPPFLAGS)
AC_SUBST(IDS_LIBS)
AC_SUBST(VEHICLE_INFOR_LIB)

AC_CONFIG_FILES([
    Makefile
    src/Makefile
])

AC_OUTPUT
