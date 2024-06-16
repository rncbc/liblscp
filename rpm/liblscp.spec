#
# spec file for package liblscp
#
# Copyright (C) 2004-2024, rncbc aka Rui Nuno Capela. All rights reserved.
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
Version:	1.0.0
Release:	1.1
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
* Wed Jun 19 2024 Rui Nuno Capela <rncbc@rncbc.org> 1.0.0
- An Unthinkable Release.
* Wed May  1 2024 Rui Nuno Capela <rncbc@rncbc.org> 0.9.91
- A Spring'24 Release Candidate 2.
* Wed Apr 10 2024 Rui Nuno Capela <rncbc@rncbc.org> 0.9.90
- A Spring'24 Release Candidate.
* Wed Jan 24 2024 Rui Nuno Capela <rncbc@rncbc.org> 0.9.12
- A Winter'24 Release.
* Sat Sep  9 2023 Rui Nuno Capela <rncbc@rncbc.org> 0.9.11
- An End-of-Summer'23 Release.
* Thu Jun  1 2023 Rui Nuno Capela <rncbc@rncbc.org> 0.9.10
- A Spring'23 Release.
* Thu Mar 23 2023 Rui Nuno Capela <rncbc@rncbc.org> 0.9.9
- An Early-Spring'23 Release.
* Wed Dec 28 2022 Rui Nuno Capela <rncbc@rncbc.org> 0.9.8
- An End-of-Year'22 Release.
* Mon Oct  3 2022 Rui Nuno Capela <rncbc@rncbc.org> 0.9.7
- An Early-Autumn'22 Release.
* Sat Apr  2 2022 Rui Nuno Capela <rncbc@rncbc.org> 0.9.6
- A Spring'22 Release.
* Sun Jan  9 2022 Rui Nuno Capela <rncbc@rncbc.org> 0.9.5
- A Winter'22 Release.
* Sun Jul  4 2021 Rui Nuno Capela <rncbc@rncbc.org> 0.9.4
- Early-Summer'21 release.
* Tue May 11 2021 Rui Nuno Capela <rncbc@rncbc.org> 0.9.3
- Spring'21 release.
* Sun Mar 14 2021 Rui Nuno Capela <rncbc@rncbc.org> 0.9.2
- End-of-Winter'21 release.
* Sun Feb  7 2021 Rui Nuno Capela <rncbc@rncbc.org> 0.9.1
- Winter'21 release.
* Thu Dec 17 2020 Rui Nuno Capela <rncbc@rncbc.org> 0.9.0
- Winter'20 release.
* Tue Mar 24 2020 Rui Nuno Capela <rncbc@rncbc.org> 0.6.2
- Spring'20 release.
* Sun Dec 22 2019 Rui Nuno Capela <rncbc@rncbc.org> 0.6.1
- Winter'19 release.
* Tue Dec 12 2017 Rui Nuno Capela <rncbc@rncbc.org> 0.6.0
- Autumn'17 release: bumped directly to 0.6.0.
* Mon Nov 14 2016 Rui Nuno Capela <rncbc@rncbc.org> 0.5.8
- Fall'16 release.
* Tue Dec 31 2013 Rui Nuno Capela <rncbc@rncbc.org> 0.5.7
- A fifth of a Jubilee release.
* Sun Feb 24 2013 Rui Nuno Capela <rncbc@rncbc.org>
- Use getaddrinfo() instead of deprecated gethostbyname().
* Sat Aug  1 2009 Rui Nuno Capela <rncbc@rncbc.org> 0.5.6
- New 0.5.6 release.
* Fri Oct 12 2007 Rui Nuno Capela <rncbc@rncbc.org> 0.5.5
- Changed client interface function, for editing channel instrument.
- New 0.5.5 release.
* Tue Oct  2 2007 Rui Nuno Capela <rncbc@rncbc.org> 0.5.4
- Added new client interface function, for editing instrument.
- New 0.5.4 release.
* Mon Jan 15 2007 Rui Nuno Capela <rncbc@rncbc.org> 0.5.3
- New 0.5.3 release.
* Thu Jan 11 2007 Rui Nuno Capela <rncbc@rncbc.org> 0.5.2
- Sampler channel effect sends control and global volume support.
- Audio routing representation changed to integer array.
- New 0.5.2 release.
* Fri Dec 22 2006 Rui Nuno Capela <rncbc@rncbc.org> 0.5.1
- Added support for new (un)subscribable events.
- Examples update.
- Prepared for 0.5.1 maintenance release.
* Sun Dec 17 2006 Rui Nuno Capela <rncbc@rncbc.org> 0.5.0
- Multi MIDI instrument maps introduced, sampler channel assignable.
- Moved on up to a brand new 0.5.0 release.
* Mon Dec  4 2006 Rui Nuno Capela <rncbc@rncbc.org> 0.4.2
- Going up to 0.4.2 fast.
* Tue Nov 28 2006 Rui Nuno Capela <rncbc@rncbc.org> 0.4.1
- Bumped directly to 0.4.1 release.
- Getting ready for the new MIDI instrument mapping features.
* Thu Jun  1 2006 Rui Nuno Capela <rncbc@rncbc.org> 0.3.3
- Take a chance for a new 0.3.3 release.
- Changed deprecated copyright attribute to license.
- Added ldconfig to post-(un)install steps.
* Mon Aug 29 2005 Rui Nuno Capela <rncbc@rncbc.org> 0.3.2
- Fixed for 0.3.2 release.
* Wed Aug 10 2005 Rui Nuno Capela <rncbc@rncbc.org> 0.3.1
- Prepare 0.3.1 release for sampler channel mute/solo support.
* Fri Jun 10 2005 Rui Nuno Capela <rncbc@rncbc.org> 0.3.0
- Prepare 0.3.0 release featuring timeout flush idiosyncrasies.
* Sun May 22 2005 Rui Nuno Capela <rncbc@rncbc.org> 0.2.9
- Prepare 0.2.9 release due to event subscription LSCP command changes.
* Fri May  6 2005 Rui Nuno Capela <rncbc@rncbc.org> 0.2.8
- Prepare 0.2.8 release in response to [bug #9].
* Thu Mar 10 2005 Rui Nuno Capela <rncbc@rncbc.org> 0.2.7
- Prepare 0.2.7 yet another bug-fix release.
* Tue Mar  1 2005 Rui Nuno Capela <rncbc@rncbc.org> 0.2.6
- Prepare 0.2.6 bug-fix release.
* Mon Feb 14 2005 Rui Nuno Capela <rncbc@rncbc.org> 0.2.5
- Prepare 0.2.5 release.
* Mon Oct 11 2004 Rui Nuno Capela <rncbc@rncbc.org> 0.2.4
- Fixed 0.2.4 release.
* Tue Sep 28 2004 Rui Nuno Capela <rncbc@rncbc.org> 0.2.3
- Fixed 0.2.3 release.
* Thu Jul 29 2004 Rui Nuno Capela <rncbc@rncbc.org> 0.2.2
- Prepare 0.2.2 release.
* Tue Jul 13 2004 Christian Schoenebeck <cuse@users.sourceforge.net>
- renamed 'liblscp.pc' to 'lscp.pc' as well as the pkg-config lib name
* Thu Jul  8 2004 Rui Nuno Capela <rncbc@rncbc.org> 0.2.1
- Prepare 0.2.1 bugfix release.
* Tue Jul  6 2004 Rui Nuno Capela <rncbc@rncbc.org> 0.2.2
- Catch up on 0.2.0 release.
* Mon Apr 26 2004 Rui Nuno Capela <rncbc@rncbc.org>
- Server stuff moved out (stays on examples source package)
* Sat Apr 24 2004 Rui Nuno Capela <rncbc@rncbc.org>
- Created initial liblscp.spec.in
