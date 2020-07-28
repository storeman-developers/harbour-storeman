#include <QtQuick>
#include <sailfishapp.h>

#include "ornclient.h"
#include "ornpm.h"
#include "ornapplication.h"
#include "ornrecentappsmodel.h"
#include "ornuserappsmodel.h"
#include "ornsearchappsmodel.h"
#include "orncategoryappsmodel.h"
#include "ornrepomodel.h"
#include "orninstalledappsmodel.h"
#include "ornproxymodel.h"
#include "orncommentsmodel.h"
#include "orncategoriesmodel.h"
#include "orntagsmodel.h"
#include "orntagappsmodel.h"
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
    qmlRegisterType<OrnRecentAppsModel>   (uri, 1, 0, "OrnRecentAppsModel");
    qmlRegisterType<OrnUserAppsModel>     (uri, 1, 0, "OrnUserAppsModel");
    qmlRegisterType<OrnSearchAppsModel>   (uri, 1, 0, "OrnSearchAppsModel");
    qmlRegisterType<OrnCategoryAppsModel> (uri, 1, 0, "OrnCategoryAppsModel");
    qmlRegisterType<OrnRepoModel>         (uri, 1, 0, "OrnRepoModel");
    qmlRegisterType<OrnInstalledAppsModel>(uri, 1, 0, "OrnInstalledAppsModel");
    qmlRegisterType<OrnProxyModel>        (uri, 1, 0, "OrnProxyModel");
    qmlRegisterType<OrnCommentsModel>     (uri, 1, 0, "OrnCommentsModel");
    qmlRegisterType<OrnCategoriesModel>   (uri, 1, 0, "OrnCategoriesModel");
    qmlRegisterType<OrnTagsModel>         (uri, 1, 0, "OrnTagsModel");
    qmlRegisterType<OrnTagAppsModel>      (uri, 1, 0, "OrnTagAppsModel");
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

    auto view = SailfishApp::createView();

    NetworkAccessManagerFactory factory;
    view->engine()->setNetworkAccessManagerFactory(&factory);

    view->setSource(SailfishApp::pathToMainQml());
    view->show();

    return QGuiApplication::exec();
}
