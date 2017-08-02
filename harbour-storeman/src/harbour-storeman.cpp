#include <QtQuick>
#include <sailfishapp.h>

#include <orn_plugin.h>
#include <ornzypp.h>
#include "networkaccessmanagerfactory.h"

Q_IMPORT_PLUGIN(OrnPlugin)

int main(int argc, char *argv[])
{
    // Have to call it for static plugins
    OrnPlugin().registerTypes();

    auto app = SailfishApp::application(argc, argv);
    app->setApplicationVersion(QStringLiteral(APP_VERSION));

    // SailfishApp does not load default id based translation file
    // if there is no a translation for the current locale
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
    auto root = view->rootContext();

    root->setContextProperty(QStringLiteral("ornZypp"), OrnZypp::instance());

    NetworkAccessManagerFactory factory;
    view->engine()->setNetworkAccessManagerFactory(&factory);

    view->setSource(SailfishApp::pathTo(QStringLiteral("qml/harbour-storeman.qml")));
    view->show();

    return app->exec();
}
