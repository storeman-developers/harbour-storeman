#pragma once

#include <QDBusAbstractInterface>

class QDBusPendingCallWatcher;

class OrnSsu : public QDBusAbstractInterface
{
public:
    explicit OrnSsu(QObject *parent = nullptr);

    void addRepo(const QString &alias, const QString &url);
    void modifyRepo(int action, const QString &alias);
};
