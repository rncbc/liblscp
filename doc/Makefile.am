CLEANFILES = doxygen-build.stamp

DOX = liblscp.doxygen
EXTRA_DIST = reference.dox
INSTIMAGES = html/doxygen.png

all-local: doxygen-build.stamp

doxygen-build.stamp: $(DOX) reference.dox \
	../lscp/thread.h ../lscp/socket.h \
	../lscp/client.h ../lscp/device.h ../lscp/event.h \
	../examples/server.h ../examples/parser.h
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
