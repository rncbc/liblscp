
AM_CFLAGS = -Wall -I@top_srcdir@

noinst_PROGRAMS = example_server example_client
noinst_HEADERS = server.h parser.h

example_client_SOURCES = example_client.c
example_client_LDADD   = @top_srcdir@/src/liblscp.la

example_server_SOURCES = example_server.c server.c parser.c
example_server_LDADD   = @top_srcdir@/src/liblscp.la
