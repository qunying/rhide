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
%build
make RHIDESRC=`pwd` LDFLAGS=-static

%install
make RHIDESRC=`pwd` install prefix=$RPM_BUILD_ROOT/usr LDFLAGS=-static

%files
/usr/bin/gpr2mak
/usr/bin/gprexp
/usr/bin/rhgdb
/usr/bin/rhide
%doc /usr/doc/rhide/COPYING
%doc /usr/doc/rhide/COPYING.RH
%doc /usr/doc/rhide/LINUX.TXT
%doc /usr/doc/rhide/README.IDE
%doc /usr/doc/rhide/RHIDE.BIN
%doc /usr/doc/rhide/VCSA.SH
%doc /usr/doc/rhide/readme.key
%doc /usr/doc/rhide/rhide.html
%doc /usr/doc/rhide/rhide.txt
/usr/info/infview.inf
/usr/info/rhide.inf
/usr/info/setedit.inf
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
%config /usr/share/rhide/clippmac.pmc
%config /usr/share/rhide/cpmacros.pmc
%config /usr/share/rhide/htmlmac.pmc
%config /usr/share/rhide/perlmac.pmc
%config /usr/share/rhide/pmacros.pmc
%config /usr/share/rhide/rhide.terminfo
%config /usr/share/rhide/rhide_.env
%config /usr/share/rhide/syntaxhl.shl
