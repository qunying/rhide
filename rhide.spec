Summary: IDE for developing like the old known Borland 3.1 IDE
Name: rhide
Version: 1.4.8
Release: 1
Copyright: GPL
Group: Development/Debuggers
Source: /home/rho/src/rhide/rhide-1.4.8.tar.gz
%description
RHIDE allows you to develop your programs in an text-based
environment like known from old Borlands`s IDE but improved
and adapted for GNU/Linux.

%prep
%setup
%build
make

%install
make install

%files
%doc README
/usr/bin/rhide

