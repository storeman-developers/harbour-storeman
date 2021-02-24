#include <qqml.h>
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlEngine>
#include <sailfishapp.h>

#include "ornclient.h"
#include "ornpm.h"
#include "ornapplication.h"
#include "ornappsmodel.h"
#include "ornsearchappsmodel.h"
#include "ornrepomodel.h"
#include "orninstalledappsmodel.h"
#include "ornproxymodel.h"
#include "orncommentsmodel.h"
#include "orncategoriesmodel.h"
#include "orntagsmodel.h"
#include "ornbookmarksmodel.h"
#include "ornbackup.h"
#include "networkaccessmanagerfactory.h"
#include "storeman.h"
#include <storeman_version.h>


void registerTypes()
{
#ifndef QT_DEBUG
    auto uri = "harbour.orn";
#else
#   define uri "harbour.orn"
#endif

    qmlRegisterType<OrnApplication>       (uri, 1, 0, "OrnApplication");
    qmlRegisterType<OrnAppsModel>         (uri, 1, 0, "OrnAppsModel");
    qmlRegisterType<OrnSearchAppsModel>   (uri, 1, 0, "OrnSearchAppsModel");
    qmlRegisterType<OrnRepoModel>         (uri, 1, 0, "OrnRepoModel");
    qmlRegisterType<OrnInstalledAppsModel>(uri, 1, 0, "OrnInstalledAppsModel");
    qmlRegisterType<OrnProxyModel>        (uri, 1, 0, "OrnProxyModel");
    qmlRegisterType<OrnCommentsModel>     (uri, 1, 0, "OrnCommentsModel");
    qmlRegisterType<OrnCategoriesModel>   (uri, 1, 0, "OrnCategoriesModel");
    qmlRegisterType<OrnTagsModel>         (uri, 1, 0, "OrnTagsModel");
    qmlRegisterType<OrnBookmarksModel>    (uri, 1, 0, "OrnBookmarksModel");
    qmlRegisterType<OrnBackup>            (uri, 1, 0, "OrnBackup");

    qmlRegisterSingletonType<OrnClient>   (uri, 1, 0, "OrnClient", OrnClient::qmlInstance);
    qmlRegisterSingletonType<OrnPm>       (uri, 1, 0, "OrnPm",     OrnPm::qmlInstance);
    qmlRegisterSingletonType<Storeman>    (uri, 1, 0, "Storeman",  Storeman::qmlInstance);

    qRegisterMetaType<QList<OrnInstalledPackage>>();
    qRegisterMetaType<QList<OrnPackageVersion>>();
}

int main(int argc, char *argv[])
{
    registerTypes();

    SailfishApp::application(argc, argv);
    QGuiApplication::setApplicationVersion(QStringLiteral(STOREMAN_VERSION));
    // TODO: Available in Qt.application object since Qt 5.9
    // QGuiApplication::setApplicationDisplayName(QStringLiteral("Storeman"));

    auto view = SailfishApp::createView();

    NetworkAccessManagerFactory factory;
    view->engine()->setNetworkAccessManagerFactory(&factory);

    view->setSource(SailfishApp::pathToMainQml());
    view->show();

    return QGuiApplication::exec();
}
