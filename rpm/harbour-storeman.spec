%global branch sfos3.2
Name:           harbour-storeman
Summary:        OpenRepos client application for SailfishOS
# The <version> tag must adhere to semantic versioning: Among multiple other
# reasons due to its use for `qmake5` in line 107.  See https://semver.org/
Version:        0.3.6
# The <release> tag comprises one of {alpha,beta,rc,release} postfixed with a
# natural number greater or equal to 1 (e.g., "beta3") and may additionally be
# postfixed with a plus character ("+"), the name of the packager and a release
# number chosen by her (e.g., "rc2+jane4").  `{alpha|beta|rc|release}`
# indicates the expected status of the software.  No other identifiers shall be
# used for any published version, but for the purpose of testing infrastructure
# other nonsensual identifiers as `adud` may be used, which do *not* trigger a
# build at GitHub and OBS, when configured accordingly; mind the sorting
# (`adud` < `alpha`).  For details and reasons, see
# https://github.com/storeman-developers/harbour-storeman/wiki/Git-tag-format
Release:        release2_%{branch}
# The Group tag should comprise one of the groups listed here:
# https://github.com/mer-tools/spectacle/blob/master/data/GROUPS
Group:          Software Management/Package Manager
License:        MIT
URL:            https://github.com/storeman-developers/%{name}
# Altering the `Vendor:` field breaks the update path on SailfishOS, see
# https://en.opensuse.org/SDB:Vendor_change_update#Disabling_Vendor_stickiness
Vendor:         meego
# The "Source0:" line below requires that the value of %%{name} is also the
# project name at GitHub and the value of `%%{release}/%%{version}` is also
# the name of a correspondingly set Git tag.  For details and reasons, see
# https://github.com/storeman-developers/harbour-storeman/wiki/Git-tag-format
Source0:        %{url}/archive/%{release}/%{version}/%{name}-%{version}.tar.gz
# Note that the rpmlintrc file must be named so according to
# https://en.opensuse.org/openSUSE:Packaging_checks#Building_Packages_in_spite_of_errors
Source99:       %{name}.rpmlintrc

# Segregate the SFOS releases range covered by each release branch:
Requires: sailfish-version >= 3.1.0
Requires: sailfish-version  < 3.3.0

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

%if 0%{?_chum}
Title: Storeman for SailfishOS
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
PackageIcon: %{url}/raw/master/icons/harbour-storeman.svg
Screenshots:
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-01.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-02.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-03.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-04.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-06.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-07.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-08.png
 - %{url}/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-09.png
Links:
  Homepage: https://openrepos.net/content/olf/storeman-installer
  Help: %{url}/issues
  Bugtracker: %{url}/issues
  Donation: https://openrepos.net/donate
%endif

# Define (S)RPM compression sensibly, also taking compatibility into account, see
# https://github.com/sailfishos-patches/patchmanager/pull/417#issuecomment-1429068156
%define _source_payload w6.gzdio
%define _binary_payload w2.xzdio

%prep
%setup -q

%build
%qmake5 VERSION=%{version}
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
