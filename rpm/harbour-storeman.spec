Name:           harbour-storeman
Summary:        OpenRepos client application for SailfishOS
Version:        0.3.3
Release:        1
Group:          Applications/System
License:        MIT
URL:            https://github.com/storeman-developers/%{name}
# The "Source0:" line below requires that the value of %%{name} is also the
# project name at GitHub and the value of %%{version} is also the name of a
# correspondingly set git-tag.
Source0:        %{url}/archive/%{version}/%{name}-%{version}.tar.gz
# Note that the rpmlintrc file must be named so according to
# https://en.opensuse.org/openSUSE:Packaging_checks#Building_Packages_in_spite_of_errors
Source99:       %{name}.rpmlintrc

# Requires: sailfish-version >= 3.1.0 for the code in the sfos3.2 branch and >= 3.3.0 for the code in all other branches.
# Requires: sailfish-version >= 4.2.0 for the changed "sharing" code for SFOS4.2's new sharing API in the sfos4.2 branch.
# Requires: sailfish-version < 3.3.0 and < 4.2.0 are only counterparts to segregate the covered SFOS releases range.

Requires:       sailfishsilica-qt5
Requires:       nemo-qml-plugin-dbus-qt5
Requires:       nemo-qml-plugin-notifications-qt5
Requires:       connman-qt5-declarative
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
BuildRequires:  pkgconfig(libsolv)
BuildRequires:  pkgconfig(packagekitqt5)
BuildRequires:  qt5-qttools-linguist
BuildRequires:  desktop-file-utils

Conflicts:      %{name}-installer
Obsoletes:      %{name}-installer

# This %%description section includes metadata for SailfishOS:Chum, see
# https://github.com/sailfishos-chum/main/blob/main/Metadata.md
%description
Storeman manages repositories and applications from OpenRepos.net
on your SailfishOS device.

%if "%{?vendor}" == "chum"
PackageName: Storeman for SailfishOS
Type: desktop-application
Categories:
 - System
 - Utility
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
  Homepage: https://openrepos.net/content/olf/storeman-installer
  Help: %{url}/issues
  Bugtracker: %{url}/issues
  Donation: https://openrepos.net/donate
%endif

%prep
%setup -q

%build
%qmake5 VERSION=%(echo %{version} | grep -Eo '^[0-9]+.[0-9]+.[0-9]+')
make %{?_smp_mflags}

%install
%qmake5_install
desktop-file-install --delete-original --dir=%{buildroot}%{_datadir}/applications \
   %{buildroot}%{_datadir}/applications/%{name}.desktop

%post
# The %%post scriptlet is deliberately run when installing *and* updating:
ssu_ur=no
ssu_lr="$(ssu lr | grep '^ - ' | cut -f 3 -d ' ')"
if echo "$ssu_lr" | grep -Fq mentaljam-obs
then
  ssu rr mentaljam-obs
  rm -f /var/cache/ssu/features.ini
  ssu_ur=yes
fi
if ! echo "$ssu_lr" | grep -Fq harbour-storeman-obs
then
  ssu ar harbour-storeman-obs 'https://repo.sailfishos.org/obs/home:/olf:/harbour-storeman/%%(release)_%%(arch)/'
  ssu_ur=yes
fi
if [ $ssu_ur = yes ]
then ssu ur
fi
# BTW, `ssu`, `rm -f`, `mkdir -p` etc. *always* return with "0" ("success"), hence
# no appended `|| true` needed to satisfy `set -e` for failing commands outside of
# flow control directives (if, while, until etc.).  Furthermore on Fedora Docs it
# is indicated that solely the final exit status of a whole scriptlet is crucial: 
# See https://docs.pagure.org/packaging-guidelines/Packaging%3AScriptlets.html
# or https://docs.fedoraproject.org/en-US/packaging-guidelines/Scriptlets/#_syntax
# committed on 18 February 2019 by tibbs ( https://pagure.io/user/tibbs ) in
# https://pagure.io/packaging-committee/c/8d0cec97aedc9b34658d004e3a28123f36404324
# Hence I have the impression, that only the main section of a spec file is
# interpreted in a shell called with the option `-e', but not the scriptlets
# (`%%pre*`, `%%post*`, `%%trigger*` and `%%file*`).
exit 0

%postun
if [ $1 = 0 ]  # Removal
then
  ssu rr harbour-storeman-obs
  rm -f /var/cache/ssu/features.ini
  ssu ur
  # Remove a %%{name}-installer log-file, if extant:
  rm -f %{_localstatedir}/log/%{name}-installer.log.txt
fi
exit 0

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/mapplauncherd/privileges.d/%{name}
%{_datadir}/dbus-1/services/harbour.storeman.service

