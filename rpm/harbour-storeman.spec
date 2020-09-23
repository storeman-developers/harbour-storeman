Name:           harbour-storeman
Summary:        OpenRepos Client for Sailfish OS
Version:        0.2.0
Release:        1
Group:          Qt/Qt
License:        MIT
URL:            https://github.com/mentaljam/harbour-storeman
Source0:        %{name}-%{version}.tar.bz2

Requires:       sailfish-version >= 2.1.4
Requires:       sailfishsilica-qt5
Requires:       nemo-qml-plugin-dbus-qt5
Requires:       nemo-qml-plugin-notifications-qt5
Requires:       connman-qt5-declarative
Requires:       libsolv0
Requires:       sailfishsecretsdaemon-secretsplugins-default
BuildRequires:  pkgconfig(sailfishapp)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(nemodbus)
BuildRequires:  pkgconfig(connman-qt5)
BuildRequires:  pkgconfig(nemonotifications-qt5)
BuildRequires:  pkgconfig(Qt5Sparql)
BuildRequires:  pkgconfig(Qt5Concurrent)
BuildRequires:  pkgconfig(sailfishsecrets)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  libsolv-devel
BuildRequires:  PackageKit-Qt5-devel
BuildRequires:  qt5-qttools-linguist
BuildRequires:  desktop-file-utils

%description
Manage repositories and apps from OpenRepos.net on your Sailfish OS device.

%define __requires_exclude ^libsolv.*$

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5 VERSION=%{version}
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install
desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications \
   %{buildroot}%{_datadir}/applications/*.desktop

%post
test -d %{_datadir}/%{name}/lib || mkdir -p %{_datadir}/%{name}/lib
test -f /usr/lib/libsolv.so.1 && ln -sf /usr/lib/libsolv.so.1 %{_datadir}/%{name}/lib/libsolv.so.0
test -f /usr/lib/libsolvext.so.1 && ln -sf /usr/lib/libsolvext.so.1 %{_datadir}/%{name}/lib/libsolvext.so.0

%files
%defattr(-,root,root,-)
%{_bindir}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/mapplauncherd/privileges.d/%{name}
%{_datadir}/dbus-1/services/harbour.storeman.service
