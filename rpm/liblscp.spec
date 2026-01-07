#
# spec file for package liblscp
#
# Copyright (C) 2004-2026, rncbc aka Rui Nuno Capela. All rights reserved.
# Copyright (C) 2007,2008,2015 Christian Schoenebeck
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

Summary:	LinuxSampler Control Protocol API library
Name:		liblscp
Version:	1.0.1
Release:	2.1
License:	LGPL-2.0-or-later
Source: 	%{name}-%{version}.tar.gz
URL:		https://www.linuxsampler.org/
#Packager:	rncbc.org

%define sover 6

BuildRequires:	coreutils
BuildRequires:	pkgconfig
BuildRequires:	glibc-devel
BuildRequires:	cmake >= 3.15
BuildRequires:	doxygen

%description
LinuxSampler control protocol API library.


%package -n %{name}%{sover}
Summary:	LinuxSampler Control Protocol API library
Group:		System/Libraries
Provides:	%{name}

%description -n %{name}%{sover}
This package is for use with the LinuxSampler audio sampling
engine / library and packages. Wraps the LinuxSampler network
protocol and offers a convenient API in form of a C library.

For further informations visit
https://www.linuxsampler.org

This package contains the header files needed for
development with liblscp. You will need this only if you
intend to compile programs that use this library.


%package devel
Summary:	LinuxSampler Control Protocol API library - development files
Group:		Development/Libraries/C and C++
Requires:	pkgconfig
Requires:	%{name}%{sover} = %{version}

%description devel
This package is for use with the LinuxSampler audio sampling
engine / library and packages. Wraps the LinuxSampler network
protocol and offers a convenient API in form of a C library.

For further informations visit
https://www.linuxsampler.org

This package contains the header files needed for
development with liblscp. You will need this only if you
intend to compile programs that use this library.


%prep
%setup -q

%build
cmake -DCMAKE_INSTALL_PREFIX=%{_prefix} -Wno-dev -B build
cmake --build build %{?_smp_mflags}

%install
DESTDIR="%{buildroot}" \
cmake --install build

%if 0%{?sle_version} == 150200 && 0%{?is_opensuse}
%post -n %{name}%{sover} -p /sbin/ldconfig
%postun -n %{name}%{sover} -p /sbin/ldconfig
%else
%ldconfig_scriptlets -n %{name}%{sover}
%endif


%files -n %{name}%{sover}
%license LICENSE
%{_libdir}/liblscp.so.%{sover}
%{_libdir}/liblscp.so.%{sover}.*

%files devel
%{_libdir}/liblscp.so
%{_libdir}/pkgconfig/lscp.pc
%dir %{_includedir}/lscp
%{_includedir}/lscp/*.h
%dir %{_datadir}/doc/%{name}
%dir %{_datadir}/doc/%{name}/html
%{_datadir}/doc/%{name}/html/*


%changelog
* Thu Mar 27 2025 Rui Nuno Capela <rncbc@rncbc.org> 1.0.1
- An Early Spring'25 Release.
* Wed Jun 19 2024 Rui Nuno Capela <rncbc@rncbc.org> 1.0.0
- An Unthinkable Release.
