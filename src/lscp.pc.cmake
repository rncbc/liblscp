prefix=@CONFIG_PREFIX@
exec_prefix=@CONFIG_PREFIX@
libdir=@CONFIG_LIBDIR@
includedir=@CONFIG_INCLUDEDIR@

Name: lscp
Description: LinuxSampler control protocol API
Version: @CONFIG_VERSION@
Libs: -L${libdir} -llscp
Cflags: -I${includedir}
