TARGET = harbour-storeman

QT += concurrent
CONFIG += sailfishapp

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT

LIBS += -lornplugin -L$$OUT_PWD/../ornplugin
PRE_TARGETDEPS += $$OUT_PWD/../ornplugin/libornplugin.a
INCLUDEPATH += ../ornplugin/src

PKGCONFIG += packagekit-qt5 libzypp

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

SOURCES += \
    src/harbour-storeman.cpp \
    src/networkaccessmanagerfactory.cpp

HEADERS += \
    src/networkaccessmanagerfactory.h

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
    qml/components/ListMenuItem.qml \
    qml/models/DevelopersModel.qml \
    qml/models/TranslatorsModel.qml

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

INSTALLS += polkit

include(translations/translations.pri)
