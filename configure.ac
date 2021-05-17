# Process this file with autoconf to produce a configure script.
AC_INIT(liblscp, 0.9.4, rncbc@rncbc.org, liblscp)

AC_CONFIG_SRCDIR(src/client.c)
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIRS([.m4])

AM_INIT_AUTOMAKE

#------------------------------------------------------------------------------------
# Rules for library version information:
#
#  1. Start with version information of `0:0:0' for each libtool library.
#  2. Update the version information only immediately before a public release of
#     your software. More frequent updates are unnecessary, and only guarantee
#     that the current interface number gets larger faster.
#  3. If the library source code has changed at all since the last update, then
#     increment revision (`c:r:a' becomes `c:r+1:a').
#  4. If any interfaces have been added, removed, or changed since the last update,
#     increment current, and set revision to 0.
#  5. If any interfaces have been added since the last public release, then increment
#     age.
#  6. If any interfaces have been removed since the last public release, then set age
#     to 0.

SHARED_VERSION_INFO="6:4:0"

AC_SUBST(SHARED_VERSION_INFO)

# Build version string.
AC_CACHE_VAL([ac_cv_build_version], [
   ac_cv_build_version=$(git describe --tags --dirty --abbrev=6 2>/dev/null)
   if test -n "$ac_cv_build_version"; then
      ac_cv_build_version=$(echo $ac_cv_build_version | sed 's/^[[^0-9]]\+//')
      ac_cv_build_version=$(echo $ac_cv_build_version | sed 's/^1_//')
      ac_cv_build_version=$(echo $ac_cv_build_version | sed 's/^[[_vV]]\+//')
      ac_cv_build_version=$(echo $ac_cv_build_version | sed 's/-g/git./')
      ac_cv_build_version=$(echo $ac_cv_build_version | sed 's/[[_|-]]/./g')
      ac_cv_build_version_extra=$(git rev-parse --abbrev-ref HEAD 2>/dev/null)
      if test "x$ac_cv_build_version_extra" != "xmaster"; then
         ac_cv_build_version="$ac_cv_build_version [[$ac_cv_build_version_extra]]"
      fi
   else
      ac_cv_build_version=$PACKAGE_VERSION
   fi
])
ac_build_version="$ac_cv_build_version"
AC_DEFINE_UNQUOTED(CONFIG_BUILD_VERSION, ["$ac_build_version"], [Build version string.])

# Sanitized version string.
AC_CACHE_VAL([ac_cv_version], [
   ac_cv_version=$(echo $PACKAGE_VERSION | sed -r 's/^([[0-9|\.]]+).*$/\1/')
])
ac_version="$ac_cv_version"
AC_DEFINE_UNQUOTED(CONFIG_VERSION, ["$ac_version"], [Version string.])
AC_SUBST(ac_version)

# Set default installation prefix.
AC_PREFIX_DEFAULT(/usr/local)
if test "x$prefix" = "xNONE"; then
   prefix=$ac_default_prefix
fi
if test "x$exec_prefix" = "xNONE"; then
   exec_prefix=$prefix
fi
eval ac_prefix=$prefix
AC_SUBST(ac_prefix)
AC_DEFINE_UNQUOTED(CONFIG_PREFIX, ["$ac_prefix"], [Default installation prefix.])

# Set default installation directories.
eval ac_libdir=$libdir
AC_SUBST(ac_libdir)
AC_DEFINE_UNQUOTED(CONFIG_LIBDIR, ["$ac_libdir"], [Default object library path.])

eval ac_includedir=$includedir
AC_SUBST(ac_includedir)
AC_DEFINE_UNQUOTED(CONFIG_INCLUDEDIR, ["$ac_includedir"], [Default headers include path.])

# Enable debugging argument option.
AC_ARG_ENABLE(debug,
   AS_HELP_STRING([--enable-debug], [enable debugging (default=no)]),
   [ac_debug="$enableval"])
if test "x$ac_debug" = "xyes"; then
   AC_DEFINE(CONFIG_DEBUG, 1, [Define if debugging is enabled.])
fi

# Checks for programs.
AC_PROG_CC
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Check for a windows build.
case $host in
   *mingw*|*cygwin*) windows=yes ;;
   *)                windows=no  ;;
esac
AM_CONDITIONAL(WINDOWS, test "x$windows" = "xyes")

# Checks for standard header files.
AC_HEADER_STDC

# Checks for standard headers and functions.
if test "x$windows" = "xno"; then
   AC_CHECK_HEADERS(
      [netdb.h arpa/inet.h netinet/tcp.h netinet/in.h sys/socket.h unistd.h],
      [], [ac_headers_h="no"])
   if test "x$ac_headers_h" = "xno"; then
      AC_MSG_ERROR([*** Standard headers not found.])
   fi
   AC_CHECK_FUNCS(
      [socket connect bind listen setsockopt getsockopt getsockname gethostbyname],
      [], [ac_funcs_c="no"])
   if test "x$ac_funcs_c" = "xno"; then
      AC_MSG_ERROR([*** Standard functions not found.])
   fi
fi

# Checks for pthread library.
AC_CHECK_LIB(pthread, pthread_create)

AC_ENABLE_STATIC(no)
AC_ENABLE_SHARED(yes)

# Checks for doxygen.
AC_CHECK_PROG(ac_doxygen, doxygen, [doc], [])
AC_SUBST(ac_doxygen)

AC_OUTPUT(Makefile src/Makefile lscp/Makefile examples/Makefile doc/Makefile doc/liblscp.doxygen lscp.pc liblscp.spec lscp/version.h)

