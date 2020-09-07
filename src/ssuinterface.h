#pragma once

#include <QDBusAbstractInterface>

class QDBusPendingCallWatcher;

class SsuInterface : public QDBusAbstractInterface
{
public:
    explicit SsuInterface(QObject *parent = nullptr);

    QDBusPendingCallWatcher *addRepoAsync(const QString &alias, const QString &url);
    void addRepo(const QString &alias, const QString &url);

    QDBusPendingCallWatcher *modifyRepoAsync(int action, const QString &alias);
    void modifyRepo(int action, const QString &alias);
};
