TARGET = harbour-storeman

QT += concurrent dbus core-private
CONFIG += sailfishapp silent

PKGCONFIG += \
    nemonotifications-qt5 \
    connman-qt5 \
    sailfishsecrets \
    mlite5

DEFINES += \
    QT_RESTRICTED_CAST_FROM_ASCII

# Write version file
VERSION_H = \
"$${LITERAL_HASH}ifndef STOREMAN_VERSION" \
"$${LITERAL_HASH}   define STOREMAN_VERSION \"$$VERSION\"" \
"$${LITERAL_HASH}endif"
write_file($$$$OUT_PWD/storeman_version.h, VERSION_H)

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT

MOC_DIR = .moc
OBJECTS_DIR = .obj

LIBS += -lsolv

INCLUDEPATH += \
    /usr/include/nemonotifications-qt5 \
    /usr/include/connman-qt5 \
    /usr/include/packagekitqt5/PackageKit \
    /usr/include/PackageKit/packagekit-qt5

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

SOURCES += \
    src/harbour-storeman.cpp \
    src/networkaccessmanagerfactory.cpp \
    src/ornappsmodel.cpp \
    src/ornrepo.cpp \
    src/ornsecrets.cpp \
    src/ornpkdaemon.cpp \
    src/ornpktransaction.cpp \
    src/ornssu.cpp \
    src/storeman.cpp \
    src/ornconst.cpp \
    src/ornutils.cpp \
    src/ornclient.cpp \
    src/orncommentsmodel.cpp \
    src/ornrepomodel.cpp \
    src/ornproxymodel.cpp \
    src/ornapplication.cpp \
    src/ornapplistitem.cpp \
    src/orncommentlistitem.cpp \
    src/ornsearchappsmodel.cpp \
    src/orncategoriesmodel.cpp \
    src/orncategorylistitem.cpp \
    src/orninstalledappsmodel.cpp \
    src/ornbookmarksmodel.cpp \
    src/ornbackup.cpp \
    src/ornpm.cpp \
    src/ornpackageversion.cpp \
    src/orntagsmodel.cpp \
    src/orntaglistitem.cpp

HEADERS += \
    src/networkaccessmanagerfactory.h \
    src/ornappsmodel.h \
    src/ornsecrets.h \
    src/ornsecrets_p.h \
    src/ornpkdaemon.h \
    src/ornpktransaction.h \
    src/ornssu.h \
    src/storeman.h \
    src/storeman_p.h \
    src/ornconst.h \
    src/ornutils.h \
    src/ornclient.h \
    src/ornclient_p.h \
    src/ornabstractlistmodel.h \
    src/orncommentsmodel.h \
    src/ornrepomodel.h \
    src/ornproxymodel.h \
    src/ornapplication.h \
    src/ornapplistitem.h \
    src/orncommentlistitem.h \
    src/ornsearchappsmodel.h \
    src/orncategoriesmodel.h \
    src/orncategorylistitem.h \
    src/orninstalledappsmodel.h \
    src/ornbookmarksmodel.h \
    src/ornbackup.h \
    src/ornpm.h \
    src/ornpm_p.h \
    src/ornpackageversion.h \
    src/orninstalledpackage.h \
    src/ornrepo.h \
    src/orntagsmodel.h \
    src/orntaglistitem.h

OTHER_FILES += \
    qml/harbour-storeman.qml \
    qml/StoremanStyles.qml \
    qml/cover/CoverPage.qml \
    qml/cover/background.svg \
    qml/pages/MainPage.qml \
    qml/pages/RecentAppsPage.qml \
    qml/pages/AppPage.qml \
    qml/pages/RepositoriesPage.qml \
    qml/pages/ScreenshotPage.qml \
    qml/pages/VotingPage.qml \
    qml/pages/ChangelogPage.qml \
    qml/pages/RepositoryPage.qml \
    qml/pages/CommentsPage.qml \
    qml/pages/TagsPage.qml \
    qml/pages/TagAppsPage.qml \
    qml/pages/SearchPage.qml \
    qml/pages/AboutPage.qml \
    qml/pages/TranslationsPage.qml \
    qml/pages/DevelopmentPage.qml \
    qml/pages/CategoriesPage.qml \
    qml/pages/CategoryPage.qml \
    qml/pages/AuthorisationDialog.qml \
    qml/pages/InstalledAppsPage.qml \
    qml/pages/BookmarksPage.qml \
    qml/pages/ErrorPage.qml \
    qml/pages/BackupsPage.qml \
    qml/pages/RestoreDialog.qml \
    qml/pages/BackupDialog.qml \
    qml/pages/SettingsPage.qml \
    qml/pages/MainPageOrderDialog.qml \
    qml/pages/LocalRpmsPage.qml \
    qml/pages/CategoriesFilterPage.qml \
    qml/pages/IntervalPickerDialog.qml \
    qml/pages/UnusedReposDialog.qml \
    qml/components/AppListDelegate.qml \
    qml/components/RatingBox.qml \
    qml/components/IconLabel.qml \
    qml/components/FancyPageHeader.qml \
    qml/components/PackageInformation.qml \
    qml/components/AppInformation.qml \
    qml/components/AppInfoLabel.qml \
    qml/components/ScreenshotsBox.qml \
    qml/components/MoreButton.qml \
    qml/components/MainPageButton.qml \
    qml/components/MainPageAppGridDelegate.qml \
    qml/components/AppPageMenu.qml \
    qml/components/RefreshMenuItem.qml \
    qml/components/ParticipantsDelegate.qml \
    qml/components/CommentDelegate.qml \
    qml/components/CommentField.qml \
    qml/components/CommentLabel.qml \
    qml/components/HtmlTagButton.qml \
    qml/components/ListMenuItem.qml \
    qml/components/BookmarkButton.qml \
    qml/components/MenuStatusLabel.qml \
    qml/components/MenuSearchItem.qml \
    qml/components/StoremanTapHint.qml \
    qml/components/StoremanHintLabel.qml \
    qml/components/StoremanTouchInteractionHint.qml \
    qml/components/TagDelegate.qml \
    qml/components/DisappearAnimation.qml \
    qml/components/BackupLabel.qml \
    qml/components/BackupOptions.qml \
    qml/components/CategoriesFilterDelegate.qml \
    qml/components/IntervalView.qml \
    qml/components/ListViewPositionAnimation.qml \
    qml/components/ShareLinkAction.qml \
    qml/models/DevelopersModel.qml \
    qml/models/TranslatorsModel.qml \
    qml/models/DummyCommentsModel.qml

OTHER_FILES += \
    scripts/gha-build.sh \
    scripts/gha-release.sh \
    scripts/update_categories.py \
    scripts/update_translators.py \
    rpm/harbour-storeman.changes \
    rpm/harbour-storeman.spec \
    harbour-storeman.desktop \
    icons/harbour-storeman.svg \
    .gitignore

privileges.files = data/harbour-storeman
privileges.path = $$INSTALL_ROOT/usr/share/mapplauncherd/privileges.d/

dbus.files = data/harbour.storeman.service
dbus.path = $$INSTALL_ROOT/usr/share/dbus-1/services

repo.files = mentaljam-obs/ssu
repo.path = $$INSTALL_ROOT/usr/share/

INSTALLS += privileges dbus repo

include(translations/translations.pri)
