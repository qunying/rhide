Summary: IDE for developing like the old known Borland 3.1 IDE
Name: rhide
Version: 1.5
Release: 1
Copyright: GPL
Group: Development/Debuggers
Source: rhide-1.5.tar.gz
Packager: Robert Hoehne <robert.hoehne@gmx.net>
Vendor: Robert Hoehne <robert.hoehne@gmx.net>
Buildroot: /usr/src/packages/BUILD
Prefix: /usr
%description
RHIDE allows you to develop your programs in an text-based
environment like known from old Borlands`s IDE but improved
and adapted for GNU/Linux. RHIDE supports nearly every compiler,
which gcc supports, and additionally also the pascal compilers
gpc and fpk. The pascal support is somewhat untested but should
work after some runtime configuration on RHIDE.

%prep
%setup
./configure
%build
make RHIDESRC=`pwd`

%install
make RHIDESRC=`pwd` install prefix=$RPM_BUILD_ROOT/usr

%post
tic /usr/share/rhide/eterm-rhide

%files
/usr/bin/gpr2mak
/usr/bin/gprexp
/usr/bin/rhgdb
/usr/bin/rhide
/usr/bin/rhidex
/usr/bin/rhgdbx
%doc /usr/share/doc/rhide/COPYING
%doc /usr/share/doc/rhide/COPYING.RH
%doc /usr/share/doc/rhide/LINUX.TXT
%doc /usr/share/doc/rhide/README.IDE
%doc /usr/share/doc/rhide/RHIDE.BIN
%doc /usr/share/doc/rhide/VCSA.SH
%doc /usr/share/doc/rhide/readme.key
%doc /usr/share/doc/rhide/rhide.txt
/usr/share/info/infview.inf
/usr/share/info/rhide.inf
/usr/share/info/setedit.inf
/usr/share/locale/cs/LC_MESSAGES/rhide.mo
/usr/share/locale/da/LC_MESSAGES/rhide.mo
/usr/share/locale/de/LC_MESSAGES/rhide.mo
/usr/share/locale/es/LC_MESSAGES/rhide.mo
/usr/share/locale/fi/LC_MESSAGES/rhide.mo
/usr/share/locale/fr/LC_MESSAGES/rhide.mo
/usr/share/locale/it/LC_MESSAGES/rhide.mo
/usr/share/locale/nl/LC_MESSAGES/rhide.mo
/usr/share/locale/no/LC_MESSAGES/rhide.mo
/usr/share/locale/pl/LC_MESSAGES/rhide.mo
/usr/share/locale/pt/LC_MESSAGES/rhide.mo
/usr/share/locale/sv/LC_MESSAGES/rhide.mo
%config /usr/share/rhide/rhide_.env
%config /usr/share/rhide/SET/clippmac.pmc
%config /usr/share/rhide/SET/cpmacros.pmc
%config /usr/share/rhide/SET/htmlmac.pmc
%config /usr/share/rhide/SET/macros.slp
%config /usr/share/rhide/SET/perlmac.pmc
%config /usr/share/rhide/SET/pmacros.pmc
%config /usr/share/rhide/SET/syntaxhl.shl
%config /usr/share/rhide/eterm-rhide
