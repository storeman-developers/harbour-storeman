#ifndef NETWORKACCESSMANAGERFACTORY_H
#define NETWORKACCESSMANAGERFACTORY_H

#include <QQmlNetworkAccessManagerFactory>

class NetworkAccessManagerFactory : public QQmlNetworkAccessManagerFactory
{
public:
    NetworkAccessManagerFactory();

    // QQmlNetworkAccessManagerFactory interface
public:
    QNetworkAccessManager *create(QObject *parent);
};

#endif // NETWORKACCESSMANAGERFACTORY_H
