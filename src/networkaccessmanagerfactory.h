#pragma once

#include <QQmlNetworkAccessManagerFactory>

class NetworkAccessManagerFactory : public QQmlNetworkAccessManagerFactory
{
public:
    NetworkAccessManagerFactory() = default;

    // QQmlNetworkAccessManagerFactory interface
public:
    QNetworkAccessManager *create(QObject *parent) override;
};
