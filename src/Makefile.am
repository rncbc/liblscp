
AM_CFLAGS = -Wall -I@top_srcdir@

lib_LTLIBRARIES = liblscp.la

liblscp_la_SOURCES = thread.c socket.c client.c device.c common.c event.c
liblscp_la_LDFLAGS = -module -version-info @SHARED_VERSION_INFO@
liblscp_la_LIBADD  = -lpthread

noinst_HEADERS = common.h
