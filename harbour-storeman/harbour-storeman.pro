TARGET = harbour-storeman

QT += concurrent dbus
CONFIG += sailfishapp

PKGCONFIG += \
    nemonotifications-qt5 \
    connman-qt5

# Write version file
VERSION_H = \
"$${LITERAL_HASH}ifndef STOREMAN_VERSION" \
"$${LITERAL_HASH}   define STOREMAN_VERSION \"$$VERSION\"" \
"$${LITERAL_HASH}endif"
write_file($$$$OUT_PWD/storeman_version.h, VERSION_H)

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT

LIBS += -lornplugin -L$$OUT_PWD/../ornplugin -lsolv
PRE_TARGETDEPS += $$OUT_PWD/../ornplugin/libornplugin.a
INCLUDEPATH += ../ornplugin/src
QML_IMPORT_PATH += ../ornplugin

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

SOURCES += \
    src/harbour-storeman.cpp \
    src/networkaccessmanagerfactory.cpp \
    src/storeman.cpp

HEADERS += \
    src/networkaccessmanagerfactory.h \
    src/storeman.h

OTHER_FILES +=  \
    qml/harbour-storeman.qml \
    qml/cover/CoverPage.qml \
    qml/pages/RecentAppsPage.qml \
    qml/pages/ApplicationPage.qml \
    qml/pages/RepositoriesPage.qml \
    qml/pages/ScreenshotPage.qml \
    qml/pages/ChangelogPage.qml \
    qml/pages/UserAppsPage.qml \
    qml/pages/CommentsPage.qml \
    qml/pages/SearchPage.qml \
    qml/pages/AboutPage.qml \
    qml/pages/TranslationsPage.qml \
    qml/pages/DevelopmentPage.qml \
    qml/pages/CategoriesPage.qml \
    qml/pages/CategoryPage.qml \
    qml/pages/DevicePage.qml \
    qml/pages/AuthorisationDialog.qml \
    qml/pages/InstalledAppsPage.qml \
    qml/pages/BookmarksPage.qml \
    qml/pages/ErrorPage.qml \
    qml/pages/BackupsPage.qml \
    qml/pages/RestoreDialog.qml \
    qml/pages/BackupDialog.qml \
    qml/pages/SettingsPage.qml \
    qml/components/AppListDelegate.qml \
    qml/components/RatingBox.qml \
    qml/components/IconLabel.qml \
    qml/components/FancyPageHeader.qml \
    qml/components/PackageInformation.qml \
    qml/components/AppInformation.qml \
    qml/components/AppInfoLabel.qml \
    qml/components/ScreenshotsBox.qml \
    qml/components/MoreButton.qml \
    qml/components/ApplicationPageMenu.qml \
    qml/components/RefreshMenuItem.qml \
    qml/components/ParticipantsDelegate.qml \
    qml/components/CommentDelegate.qml \
    qml/components/CommentField.qml \
    qml/components/CommentLabel.qml \
    qml/components/HtmlTagButton.qml \
    qml/components/ListMenuItem.qml \
    qml/components/BookmarkButton.qml \
    qml/components/MenuStatusLabel.qml \
    qml/components/StoremanHint.qml \
    qml/components/StoremanHintLabel.qml \
    qml/models/DevelopersModel.qml \
    qml/models/TranslatorsModel.qml \
    qml/models/DummyCommentsModel.qml

OTHER_FILES += \
    rpm/harbour-storeman.changes \
    rpm/harbour-storeman.spec \
    rpm/harbour-storeman.yaml \
    harbour-storeman.desktop \
    icons/harbour-storeman.svg \
    .gitignore

RESOURCES += \
    harbour-storeman.qrc

polkit.files = rpm/50-harbour-storeman-packagekit.pkla
polkit.path = $$INSTALL_ROOT/var/lib/polkit-1/localauthority/50-local.d

dbus.files = rpm/harbour.storeman.service
dbus.path = $$INSTALL_ROOT/usr/share/dbus-1/services

INSTALLS += polkit dbus

include(translations/translations.pri)
