CLEANFILES = doxygen-build.stamp

DOX = liblscp.doxygen

INSTIMAGES = html/doxygen.png

all-local: doxygen-build.stamp

doxygen-build.stamp: $(DOX) reference.dox \
	@top_srcdir@/lscp/thread.h @top_srcdir@/lscp/socket.h \
	@top_srcdir@/lscp/client.h @top_srcdir@/lscp/device.h \
	@top_srcdir@/examples/server.h @top_srcdir@/examples/parser.h
	@echo '*** Running doxygen ***'
	doxygen $(DOX)
	touch doxygen-build.stamp

clean-local:
	if test -d html;  then rm -fr html;  fi
	if test -d latex; then rm -fr latex; fi
	if test -d man;   then rm -fr man;   fi

distclean-local: clean
	rm -f *.stamp || true
	if test -d html; then rm -rf html; fi
