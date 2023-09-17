# ![Storeman icon 86x86](https://github.com/storeman-developers/harbour-storeman/raw/master/icons/86x86/harbour-storeman.png) &nbsp; **Storeman**

OpenRepos client application for SailfishOS

[![Release](https://img.shields.io/github/release/storeman-developers/harbour-storeman.svg)](https://github.com/storeman-developers/harbour-storeman/releases)

## Feature set

### Working with [OpenRepos](https://openrepos.net/)
#### Discover applications
- Search for applications
- List recently updated applications
- List categories and applications by categories
- All lists are scrollable while online data is loaded in the background
- Show application info similarly to the Jolla store client
- Show categories on the applications page - tap on a category label to show all related applications
#### Commenting
- List and manage comments for applications (add, edit and reply)
- Tap on the "Reply to" label on the comments list to navigate to the original comment
- Use button panel to add HTML tags when typing comments (hint: select text and then click a button to wrap the text with a tag)
#### Rating
- Rate applications

### Working with your device
- List and manage repositories
- List and manage applications installed from OpenRepos
- Set bookmarks for applications
- Backup list of installed applications
- Find and manage local RPM files

## Installing Storeman

The [Storeman Installer](https://github.com/storeman-developers/harbour-storeman-installer#readme) for installing Storeman proper [is available at OpenRepos](https://openrepos.net/content/olf/storeman-installer).

## Important notes

* If you experience issues when installing, removing or updating packages after a SailfishOS upgrade, try running `devel-su pkcon refresh` in a terminal app.
* Starting with version 0.2.9, Storeman is built by the help of the SailfishOS-OBS and initially installed by the Storeman Installer (or manually).  To update from Storeman < 0.2.9 (needs SailfishOS ≥ 3.1.0), one should reinstall Storeman via the Storeman Installer (which installs the current Storeman release, and since Storeman Installer 1.3.0 automatically removes a Storeman < 0.3.0 before that) or manually remove Storeman < 0.2.9 and install Storeman ≥ 0.3.0.  After an initial installation of Storeman ≥ 0.3.0, further updates of Storeman will be performed within Storeman, as usual.
* Before software can be build for a SailfishOS release at the SailfishOS-OBS, Jolla must create a [corresponding "download on demand (DoD)" OBS-repository](https://build.merproject.org/project/subprojects/sailfishos).  It may take a little time after a new SailfishOS release is published before the corresponding "DoD" repository is being made available, during which installing Storeman by the Storeman Installer or updating Storeman by itself on a device with the new SailfishOS release already installed does not work, because Storeman cannot be compiled for this new SailfishOS release at the Sailfish-OBS, yet; consequently this is always the case for "closed beta (cBeta)" releases of SailfishOS.  In such a situation one has to manually download Storeman built for the last prior SailfishOS "general availability (GA)" release (e.g., from [its releases section at GitHub](https://github.com/storeman-developers/harbour-storeman/releases) or [the SailfishOS-OBS](https://build.merproject.org/project/show/home:olf:harbour-storeman)), then install or update Storeman via `pkcon install-local <downloaded RPM file>`, and hope that there is no change in the new SailfishOS release which breaks Storeman; if there is, please report that soon at [Storeman's issue tracker](https://github.com/storeman-developers/harbour-storeman/issues).
* Disclaimer: Storeman and its installer may still have flaws, kill your kittens or break your SailfishOS installation!  Although this is very unlikely after years of testing by many users, new flaws may be introduced in any release (as for any software).  Mind that the license you implicitly accept by using Storeman or Storeman Installer excludes any liability.

## Translating Storeman (l10n / i18n)

If you want to translate Storeman to a language it does not support yet or enhance an existing translation, please [read the translations-README](https:./translations#readme).

## Screenshots of Storeman

|     |     |     |     |
| --- | --- | --- | --- |
| ![RecentAppsPage](https://github.com/storeman-developers/harbour-storeman/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-01.png) | ![AppPage](https://github.com/storeman-developers/harbour-storeman/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-02.png) | ![CommentsPage](https://github.com/storeman-developers/harbour-storeman/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-03.png) | ![Commenting](https://github.com/storeman-developers/harbour-storeman/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-04.png) |
| ![RepositoriesPage](https://github.com/storeman-developers/harbour-storeman/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-06.png) | ![InstalledAppsPage](https://github.com/storeman-developers/harbour-storeman/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-07.png) | ![BookmarksPage](https://github.com/storeman-developers/harbour-storeman/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-08.png) | ![RepositoryPage](https://github.com/storeman-developers/harbour-storeman/raw/master/.xdata/screenshots/screenshot-screenshot-storeman-09.png) |

## Credits

- [Petr Tsymbarovich (mentaljam / osetr)](https://github.com/mentaljam)
- [olf (Olf0)](https://github.com/Olf0)
- [Matti Viljanen (direc85)](https://github.com/direc85)
- [Björn Bidar (Thaodan)](https://github.com/Thaodan)
- [Dmitry Gerasimov (dseight)](https://github.com/dseight)
- [elros34](https://github.com/elros34)
- [Christoph (inta)](https://github.com/inta)
- [Miklós Márton (martonmiklos)](https://github.com/martonmiklos)
- [OpenRepos service](https://openrepos.net/content/basil/about-openrepos) by [Basil Semuonov (custodian / thecust)](https://github.com/custodian)
- [Storeman icon](https://github.com/storeman-developers/harbour-storeman/raw/master/icons/harbour-storeman.svg) by [Laurent Chambon (Laurent_C)](https://mastodon.social/@Laurent_C)
