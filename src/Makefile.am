
AM_CFLAGS = -Wall -I.. -I$(srcdir)/..

lib_LTLIBRARIES = liblscp.la

liblscp_la_SOURCES = thread.c socket.c client.c device.c common.c event.c
liblscp_la_LDFLAGS = -no-undefined -version-info @SHARED_VERSION_INFO@
if WINDOWS
liblscp_la_LIBADD  = -lws2_32
else
liblscp_la_LIBADD  = -lpthread
endif

noinst_HEADERS = common.h
