#ifndef NETWORKACCESSMANAGERFACTORY_H
#define NETWORKACCESSMANAGERFACTORY_H

#include <QQmlNetworkAccessManagerFactory>

class NetworkAccessManagerFactory : public QQmlNetworkAccessManagerFactory
{
public:
    NetworkAccessManagerFactory() = default;

    // QQmlNetworkAccessManagerFactory interface
public:
    QNetworkAccessManager *create(QObject *parent) override;
};

#endif // NETWORKACCESSMANAGERFACTORY_H
