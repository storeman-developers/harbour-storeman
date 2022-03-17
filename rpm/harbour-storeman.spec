Name:           harbour-storeman
Summary:        OpenRepos client application for SailfishOS
Version:        0.2.12
Release:        1
Group:          Qt/Qt
License:        MIT
URL:            https://github.com/storeman-developers/harbour-storeman
Source0:        %{name}-%{version}.tar.bz2

# Requires: sailfish-version >= 3.1.0 for the code in the sfos3.2 branch and >= 3.3.0 for the code in all other branches.
# Note that >= 4.2.0 only exists for the changed "sharing" code (for SFOS4.2's new sharing API) in the sfos4.2 branch.
# Requires: sailfish-version < 3.3.0 and < 4.2.0 are only counterparts to segregate the covered SFOS releases range.
# But these two "Requires:" have become superfluous since Storeman is distributed via Sailfish-OBS, as it allows to set
# segregated target release ranges to build Storeman from its three release branches (sfos3.2, sfos3.3 and sfos4.2).

Requires:       sailfishsilica-qt5
Requires:       nemo-qml-plugin-dbus-qt5
Requires:       nemo-qml-plugin-notifications-qt5
Requires:       connman-qt5-declarative
Requires:       libsolv
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

Conflicts:      %{name}-installer
Obsoletes:      %{name}-installer

%description
Storeman manages repositories and applications from OpenRepos.net on your SailfishOS device.

# This description section includes metadata for SailfishOS:Chum, see
# https://github.com/sailfishos-chum/main/blob/main/Metadata.md
%if "%{?vendor}" == "chum"
PackageName: Storeman for SailfishOS
Type: desktop-application
Categories:
 - Utilities
 - System
 - Network
 - Settings
 - PackageManager
DeveloperName: Storeman developers (mentaljam)
Custom:
  Repo: %{url}
Icon: %{url}/raw/master/icons/harbour-storeman.svg
Screenshots:
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-01.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-02.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-03.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-04.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-06.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-07.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-08.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-09.png
Url:
  Homepage: %{url}
  Help: %{url}/issues
  Bugtracker: %{url}/issues
%endif

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5 VERSION=%(echo %{version} | grep -Eo '^[0-9]+.[0-9]+.[0-9]+')
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install
desktop-file-install --delete-original --dir %{buildroot}%{_datadir}/applications %{buildroot}%{_datadir}/applications/*.desktop

%posttrans
ssu rr mentaljam-obs
rm -f /var/cache/ssu/features.ini
ssu ar harbour-storeman-obs 'https://repo.sailfishos.org/obs/home:/olf:/harbour-storeman/%%(release)_%%(arch)/'
ssu ur

%postun
ssu rr harbour-storeman-obs
rm -f /var/cache/ssu/features.ini
ssu ur

%files
%defattr(-,root,root,-)
%{_bindir}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/mapplauncherd/privileges.d/%{name}
%{_datadir}/dbus-1/services/harbour.storeman.service
