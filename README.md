# Storeman

OpenRepos client application for SailfishOS

[![Release](https://img.shields.io/github/release/storeman-developers/harbour-storeman.svg)](https://github.com/storeman-developers/harbour-storeman/releases)

## Current features

### Working with OpenRepos.net
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

## Installation of Storeman

The Storeman Installer for installing Storeman proper is available at [OpenRepos.net](https://openrepos.net/content/osetr/storeman-installer).

## Contributors to Storeman

- [Petr Tsymbarovich](https://github.com/mentaljam) (mentaljam / osetr)
- [Christoph](https://github.com/inta) (inta)
- [Matti Viljanen](https://github.com/direc85) (direc85)
- [olf](https://github.com/Olf0) (Olf0)
- OpenRepos service by [Basil Semuonov](https://github.com/custodian) (custodian)
- Application icon by [Laurent_C](https://openrepos.net/users/laurentc) (laurentc)

## Translating Storeman ("l10n" / "i18n")

If you want to translate Storeman to a language it does not support yet or enhance an existing translation, please [read the translations-README](translations/README.md).

## Screenshots of Storeman

|     |     |     |     |
| --- | --- | --- | --- |
| ![RecentAppsPage](https://github.com/storeman-developers/harbour-storeman/blob/devel/.xdata/screenshots/screenshot-screenshot-storeman-01.png) | ![AppPage](https://github.com/storeman-developers/harbour-storeman/blob/devel/.xdata/screenshots/screenshot-screenshot-storeman-02.png) | ![CommentsPage](https://github.com/storeman-developers/harbour-storeman/blob/devel/.xdata/screenshots/screenshot-screenshot-storeman-03.png) | ![Commenting](https://github.com/storeman-developers/harbour-storeman/blob/devel/.xdata/screenshots/screenshot-screenshot-storeman-04.png) |
| ![RepositoriesPage](https://github.com/storeman-developers/harbour-storeman/blob/devel/.xdata/screenshots/screenshot-screenshot-storeman-06.png) | ![InstalledAppsPage](https://github.com/storeman-developers/harbour-storeman/blob/devel/.xdata/screenshots/screenshot-screenshot-storeman-07.png) | ![BookmarksPage](https://github.com/storeman-developers/harbour-storeman/blob/devel/.xdata/screenshots/screenshot-screenshot-storeman-08.png) | ![RepositoryPage](https://github.com/storeman-developers/harbour-storeman/blob/devel/.xdata/screenshots/screenshot-screenshot-storeman-09.png) |

## Important notes

* If you have any troubles with installing, removing or updating packages after a SailfishOS upgrade, try running `devel-su pkcon refresh` in the terminal app.
* Starting with version 0.2.9, Storeman is built by the help of the SailfishOS-OBS and initially installed by the Storeman Installer (or manually).  To update from Storeman < 0.2.9, one must remove ("uninstall") Storeman *before* installing the Storeman Installer or manually installing Storeman ≥ 0.2.9.  After an initial installation of Storeman ≥ 0.2.9, further updates of Storeman will be performed within Storeman, as usual. 
* Before software can be built for a SailfishOS release at the SailfishOS-OBS, Jolla must create a corresponding "download on demand (DoD)" OBS-repository.  It often takes some time after a new "general availability (GA)" SailfishOS release is published before the corresponding "DoD" repository is being made available, during which installing or updating Storeman by the Storeman Installer or Storeman's self-updating on a device with the new SailfishOS release already installed will fail; consequently this is always the case during the "closed beta (cBeta)" and "early access (EA)" phases of a new SailfishOS release.  Hence one has to either manually set the last prior SailfishOS GA release in the SailfishOS:Chum GUI application or manually download and install or update Storeman built for the last prior SailfishOS GA release, then.
* **Disclaimer:** Storeman and Storeman Installer may still have flaws, kill your kittens or break your SailfishOS installation!  Although this is very unlikely after years of testing by many users, new flaws may be introduced in any release (as for any software).  Be aware, that the license you implicitly accept by using Storeman excludes any liability.


