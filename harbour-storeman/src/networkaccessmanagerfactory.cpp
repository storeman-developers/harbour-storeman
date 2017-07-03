#include "networkaccessmanagerfactory.h"

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QStandardPaths>
#include <QDebug>

NetworkAccessManagerFactory::NetworkAccessManagerFactory()
{

}

QNetworkAccessManager *NetworkAccessManagerFactory::create(QObject *parent)
{
    auto manager = new QNetworkAccessManager(parent);
    auto diskCache = new QNetworkDiskCache(manager);
    diskCache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
                                 .append("/network"));
    qDebug() << "Setting qml network cache dir to" << diskCache->cacheDirectory();
    manager->setCache(diskCache);
    return manager;
}
