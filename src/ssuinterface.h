#pragma once

#include <QDBusAbstractInterface>

class QDBusPendingCallWatcher;

class SsuInterface : public QDBusAbstractInterface
{
public:
    explicit SsuInterface(QObject *parent = nullptr);

    void addRepo(const QString &alias, const QString &url);
    void modifyRepo(int action, const QString &alias);
};
