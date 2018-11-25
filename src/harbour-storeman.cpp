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


int main(int argc, char *argv[])
{
    // Register types
    const char *uri = "harbour.orn";

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

    auto app = SailfishApp::application(argc, argv);
    app->setApplicationVersion(QStringLiteral(STOREMAN_VERSION));

    // SailfishApp does not load default id based translation file
    // if there is no a translation for the current locale
    // UPD: the issue was fixed - waiting for a new libsailfishapp version
    {
        auto *translator = new QTranslator(app);
        auto trPath = SailfishApp::pathTo(QStringLiteral("translations")).toLocalFile();
        auto appName = app->applicationName();
        // Check if translations have been already loaded
        if (!translator->load(QLocale::system(), appName, "-", trPath))
        {
            // Fallback default locale to en
            QLocale::setDefault(QLocale(QLocale::AnyLanguage));
            // Load default translations
            translator->load(appName, trPath);
            app->installTranslator(translator);
        }
        else
        {
            translator->deleteLater();
        }
    }

    auto view = SailfishApp::createView();

    NetworkAccessManagerFactory factory;
    view->engine()->setNetworkAccessManagerFactory(&factory);

    view->setSource(SailfishApp::pathToMainQml());
    view->show();

    return app->exec();
}
