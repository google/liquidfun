#
# spec file for package freeglut (Version 2.6.0)
#
# Copyright (c) 2005 SUSE LINUX Products GmbH, Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments via http://www.suse.de/feedback/
#

# norootforbuild
# usedforbuild    Mesa Mesa-devel aaa_base acl attr audit-libs autoconf automake bash bind-libs bind-utils binutils bison bzip2 coreutils cpio cpp cracklib cvs cyrus-sasl db diffutils e2fsprogs expat file filesystem fillup findutils flex fontconfig fontconfig-devel gawk gcc gcc-c++ gdbm gdbm-devel gettext gettext-devel glibc glibc-devel glibc-locale gpm grep groff gzip info insserv klogd less libacl libattr libcom_err libdrm libdrm-devel libgcc libnscd libstdc++ libstdc++-devel libtool libxcrypt libzio m4 make man mktemp module-init-tools ncurses ncurses-devel net-tools netcfg openldap2-client openmotif-libs openssl pam pam-modules patch perl permissions popt procinfo procps psmisc pwdutils rcs readline rpm sed strace sysvinit tar tcpd texinfo timezone unzip util-linux vim xorg-x11-devel xorg-x11-libs zlib zlib-devel

BuildRequires:  Mesa-devel gcc-c++ libdrm-devel

Name:         freeglut
Summary:      Freely Licensed Alternative to the GLUT Library
Version:      2.6.0
Release:      1
URL:          http://freeglut.sourceforge.net/
Source0:      http://switch.dl.sourceforge.net/sourceforge/freeglut/freeglut-%{version}.tar.gz
# NOTE: SuSE adds the original GLUT man pages. If you don't have the file
# below, comment that line out and remove "-b1" from the setup macro.
Source1:      glutman.tar.bz2
License:      X11/MIT
Provides:     mesaglut
Obsoletes:    mesaglut
Group:        System/Libraries
BuildRoot:    %{_tmppath}/%{name}-%{version}-build

%description
freeglut is a completely Open Source alternative to the OpenGL Utility
Toolkit (GLUT) library. GLUT was originally written by Mark Kilgard to
support the sample programs in the second edition OpenGL Redbook. Since
then, GLUT has been used in a wide variety of practical applications
because it is simple, universally available, and highly portable.

GLUT (and freeglut) allow the user to create and manage windows
containing OpenGL contexts and also read the mouse, keyboard, and
joystick functions on a wide range of platforms.



Authors:
--------
    Pawel W. Olszta
    Andreas Umbach
    Steve Baker

%package devel
Summary:      Development Package for freeglut (GLUT Library)
Group:        Development/Libraries/X11
Requires:     %{name} = %{version}-%{release}
Provides:     mesaglut-devel
Obsoletes:    mesaglut-devel

%description devel
Freeglut (GLUT library) development package.



%debug_package
%prep
%setup -q -b1

%build
./autogen.sh
CFLAGS="$RPM_OPT_FLAGS" \
  ./configure --prefix=/usr --libdir=%_libdir
make %{?jobs:-j%jobs}

%install
rm -rf $RPM_BUILD_ROOT

%makeinstall 
# GLUT manual pages
touch man-pages
if test -d ../glut-3.7 ; then
  mkdir -p ${RPM_BUILD_ROOT}/%{_mandir}/man3
  dir=`pwd`
  pushd ../glut-3.7
    for i in man/glut/glut*; do
      install -m 644 $i ${RPM_BUILD_ROOT}/%{_mandir}/man3/`basename $i man`3
      gzip -9 ${RPM_BUILD_ROOT}/%{_mandir}/man3/`basename $i man`3
      echo %{_mandir}/man3/`basename $i man`3.gz >> ${dir}/man-pages
    done
  popd
fi

%post
%run_ldconfig

%postun
%run_ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_libdir}/libglut.so.*

%files devel -f man-pages
%defattr(-,root,root)
%{_includedir}/GL/*
%{_libdir}/libglut.a
%{_libdir}/libglut.la
%{_libdir}/libglut.so

%changelog -n freeglut
* Sun Apr 05 2009 - sven.panne@aedion.de
- updated to release 2.6.0
* Sat Aug 05 2006 - sven.panne@aedion.de
- Updated build requirements for SuSE 10.1
* Mon Jun 13 2005 - sven.panne@aedion.de
- updated to release 2.4.0
- use autogen.sh
- no need to remove demos
- conditionalized handling of GLUT manual pages
* Thu Apr 29 2004 - coolo@suse.de
- build parallel if available
- build as user
* Mon Dec 15 2003 - sndirsch@suse.de
- updated to release 2.2.0
  * Mousewheel Support for Win32 & X11
  * More unified Win32/*nix behavior and code
  * Code style cleanups
  * Visibility support in Win32
  * Many other Good Bug Fixes
* Sat Dec 06 2003 - sndirsch@suse.de
- updated to release 2.0.1
  * fixes a number of bugs with menus
* Fri Oct 10 2003 - adrian@suse.de
- fix build for mips
- satisfy configure checks
* Thu Oct 02 2003 - sndirsch@suse.de
- provides/obsoletes mesaglut/mesglut-devel now
* Tue Sep 30 2003 - sndirsch@suse.de
- removed demos
* Tue Sep 30 2003 - sndirsch@suse.de
- added GLUT manual pages
* Mon Sep 29 2003 - sndirsch@suse.de
- updated to final release 2.0.0
* Thu Aug 14 2003 - sndirsch@suse.de
- added GameMode fix (GameModeFixes-4.dif)
* Thu Aug 07 2003 - sndirsch@suse.de
- created package
