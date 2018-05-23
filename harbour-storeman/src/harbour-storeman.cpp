#include <QtQuick>
#include <sailfishapp.h>

#include <orn_plugin.h>
#include "networkaccessmanagerfactory.h"
#include "storeman.h"
#include <storeman_version.h>

Q_IMPORT_PLUGIN(OrnPlugin)

int main(int argc, char *argv[])
{
    // Have to call it for static plugins
    OrnPlugin().registerTypes();
    qmlRegisterSingletonType<Storeman>(
                "harbour.orn", 1, 0, "Storeman", Storeman::qmlInstance);

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
            // Load default translations if not
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

    view->setSource(SailfishApp::pathTo(QStringLiteral("qml/harbour-storeman.qml")));
    view->show();

    return app->exec();
}
