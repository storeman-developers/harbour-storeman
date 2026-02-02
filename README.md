# ![Storeman icon 86x86](./icons/86x86/harbour-storeman.png?raw=true) &nbsp; **Storeman**
**OpenRepos client application for SailfishOS**

<sup><sub>`Current`</sub></sup>[![Current release](https://img.shields.io/github/release/storeman-developers/harbour-storeman.svg)](https://github.com/storeman-developers/harbour-storeman/releases)

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
* Starting with version 0.2.9, Storeman is built by the help of the SailfishOS-OBS and initially installed by the Storeman Installer (or manually).  To update from Storeman < 0.2.9 (requires SailfishOS ≥ 3.1.0), one should either reinstall Storeman via the Storeman Installer (which installs the current Storeman release from the SailfishOS-OBS for SailfishOS ≥ 3.4.0, and since Storeman Installer 1.3.0 automatically removes a Storeman < 0.3.0 before that), or manually remove Storeman < 0.2.9 and install Storeman ≥ 0.3.0.  After an initial installation of Storeman ≥ 0.3.0, further updates of Storeman will be performed within Storeman, as usual.
* Before software can be build for a new SailfishOS release at the SailfishOS-OBS, Jolla must create a [corresponding "download on demand (DoD)" OBS-repository](https://build.sailfishos.org/project/subprojects/sailfishos).  Before that is employed by Jolla, Storeman's download repository for this new SailfishOS release will usually be mapped to the prior SailfishOS release by [its OBS-meta-configuration](https://build.sailfishos.org/project/meta/home:olf:harbour-storeman), in hope that there is no change in the new SailfishOS release which breaks Storeman; if there is, please report that soon at [Storeman's issue tracker](https://github.com/storeman-developers/harbour-storeman/issues).
* Disclaimer: Storeman and its installer may still have flaws, kill your kittens or break your SailfishOS installation!  Although this is very unlikely after years of testing by many users, new flaws may be introduced in any release (as for any software).  Mind that the license you implicitly accept by using Storeman or Storeman Installer excludes any liability.

## Translating Storeman (l10n / i18n)

If you want to translate Storeman to a language it does not support yet or enhance an existing translation, please [read the translations-README](./translations#readme).

## Screenshots of Storeman

|       |       |       |       |
| :---: | :---: | :---: | :---: |
|       |       |       |       |
| ![RecentAppsPage](./.xdata/screenshots/screenshot-screenshot-storeman-01.png?raw=true) | ![AppPage](./.xdata/screenshots/screenshot-screenshot-storeman-02.png?raw=true) | ![CommentsPage](./.xdata/screenshots/screenshot-screenshot-storeman-03.png?raw=true) | ![Commenting](./.xdata/screenshots/screenshot-screenshot-storeman-04.png?raw=true) |
| <sub>Recently&nbsp;updated&nbsp;Apps</sub> | <sub>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;App&nbsp;details&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</sub> | <sub>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;App's&nbsp;comments&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</sub> | <sub>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Commenting&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</sub> |
|       |       |       |       |
|       |       |       |       |
| ![RepositoriesPage](./.xdata/screenshots/screenshot-screenshot-storeman-06.png?raw=true) | ![InstalledAppsPage](./.xdata/screenshots/screenshot-screenshot-storeman-07.png?raw=true) | ![BookmarksPage](./.xdata/screenshots/screenshot-screenshot-storeman-08.png?raw=true) | ![RepositoryPage](./.xdata/screenshots/screenshot-screenshot-storeman-09.png?raw=true) |
| <sub>Manage repositories</sub> | <sub>Installed Apps</sub> | <sub>Bookmarked Apps</sub> | <sub>Repository details</sub> |
|       |       |       |       |

## [Contributors](https://github.com/storeman-developers/harbour-storeman/graphs/contributors)
Note that translators are not listed here, but [there](https://github.com/storeman-developers/harbour-storeman/blob/master/qml/models/TranslatorsModel.qml).
- [Petr Tsymbarovich (mentaljam / osetr)](https://github.com/mentaljam)
- [olf (Olf0)](https://github.com/Olf0)
- [Peter G. (nephros)](https://github.com/nephros)
- [Matti Viljanen (direc85)](https://github.com/direc85)
- [Björn Bidar (Thaodan)](https://github.com/Thaodan)
- [citronalco](https://github.com/citronalco)
- [Dmitry Gerasimov (dseight)](https://github.com/dseight)
- [elros34](https://github.com/elros34)
- [Christoph (inta)](https://github.com/inta)
- [Miklós Márton (martonmiklos)](https://github.com/martonmiklos)
- [OpenRepos service](https://openrepos.net/content/basil/about-openrepos) by [Basil Semuonov (custodian / thecust)](https://github.com/custodian)
- [Storeman icon](./icons/harbour-storeman.svg) by [Laurent Chambon (Laurent_C)](https://mastodon.social/@Laurent_C)
