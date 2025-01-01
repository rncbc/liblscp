cmake_minimum_required (VERSION 3.13)

project (liblscp
  VERSION 1.0.0
  DESCRIPTION "LinuxSampler Control Protocol API library"
  HOMEPAGE_URL "https://qsampler.sourceforge.io"
  LANGUAGES C)

set (PROJECT_COPYRIGHT "Copyright (C) 2004-2025, rncbc aka Rui Nuno Capela. All rights reserved.")
set (PROJECT_DOMAIN    "linuxsampler.org")

execute_process (
  COMMAND git describe --tags --dirty --abbrev=6
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_DESCRIBE_OUTPUT
  RESULT_VARIABLE GIT_DESCRIBE_RESULT
  OUTPUT_STRIP_TRAILING_WHITESPACE)
if (GIT_DESCRIBE_RESULT EQUAL 0)
  set (GIT_VERSION "${GIT_DESCRIBE_OUTPUT}")
  string (REGEX REPLACE "^[^0-9]+" "" GIT_VERSION "${GIT_VERSION}")
  string (REGEX REPLACE "-g"   "git." GIT_VERSION "${GIT_VERSION}")
  string (REGEX REPLACE "[_|-]"   "." GIT_VERSION "${GIT_VERSION}")
  execute_process (
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_REVPARSE_OUTPUT
    RESULT_VARIABLE GIT_REVPARSE_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if (GIT_REVPARSE_RESULT EQUAL 0 AND NOT GIT_REVPARSE_OUTPUT STREQUAL "main")
    set (GIT_VERSION "${GIT_VERSION} [${GIT_REVPARSE_OUTPUT}]")
  endif ()
  set (PROJECT_VERSION "${GIT_VERSION}")
endif ()


set (CONFIG_PREFIX "${CMAKE_INSTALL_PREFIX}")

include (GNUInstallDirs)
set (CONFIG_LIBDIR  "${CONFIG_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set (CONFIG_INCLUDEDIR "${CONFIG_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")


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
#
#set (BUILD_SHARED_LIBS ON)

set (SHARED_VERSION_CURRENT  6)
set (SHARED_VERSION_AGE      0)
set (SHARED_VERSION_REVISION 6)
set (SHARED_VERSION_INFO "${SHARED_VERSION_CURRENT}.${SHARED_VERSION_AGE}.${SHARED_VERSION_REVISION}")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
  set (CONFIG_DEBUG 1)
  set (CONFIG_BUILD_TYPE "debug")
else ()
  set (CONFIG_DEBUG 0)
  set (CONFIG_BUILD_TYPE "release")
  set (CMAKE_BUILD_TYPE "Release")
endif ()

set (CONFIG_PREFIX "${CMAKE_INSTALL_PREFIX}")

include (GNUInstallDirs)
set (CONFIG_BINDIR  "${CONFIG_PREFIX}/${CMAKE_INSTALL_BINDIR}")
set (CONFIG_LIBDIR  "${CONFIG_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set (CONFIG_DATADIR "${CONFIG_PREFIX}/${CMAKE_INSTALL_DATADIR}")
set (CONFIG_INCLUDEDIR "${CONFIG_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")

#
# No special build options...
#

# Fix for new CMAKE_REQUIRED_LIBRARIES policy.
if (POLICY CMP0075)
  cmake_policy (SET CMP0075 NEW)
endif ()

include (CheckIncludeFiles)
include (CheckFunctionExists)

# Checks for standard header files.
if (UNIX AND NOT APPLE)
  check_include_files ("stdlib.h;string.h;netdb.h;arpa/inet.h;netinet/tcp.h;netinet/in.h;sys/socket.h;unistd.h" HAVE_HEADERS_H)
  if (NOT HAVE_HEADERS_H)
    message (FATAL_ERROR "*** Standard headers not found.")
  endif ()
endif ()

# Checks for standard functions.
if (UNIX AND NOT APPLE)
  set (FUNCS "strdup;strtok_r;memset;memmove;socket;connect;bind;listen;setsockopt;getsockopt;getsockname;gethostbyname")
  foreach (FUNC ${FUNCS})
     check_function_exists (${FUNC} HAVE_${FUNC}_C)
     if (NOT HAVE_${FUNC}_C)
       message (FATAL_ERROR "*** Standard functions not found.")
     endif ()
  endforeach ()
endif ()

#
# No additional packages or libraries...
#

add_subdirectory (src)
add_subdirectory (doc)
add_subdirectory (examples)

