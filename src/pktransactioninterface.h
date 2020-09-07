#pragma once

#include "ornutils.h"

#include <QDBusAbstractInterface>

class PkTransactionInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    explicit PkTransactionInterface(const QString &service, const QString &path, QObject *parent = nullptr);

    void resolve(const QStringList &names);
    void installPackages(const QStringList &ids);
    void removePackages(const QStringList &ids, bool autoremove = false);

    void installFiles(const QStringList &files);

    void repoRefreshNow(const QString &alias, const QString &force);
    void repoRefreshNow(const QString &alias, bool force = false) {
        repoRefreshNow(alias, OrnUtils::stringify(force));
    }

    void refreshCache(bool force = false);    
    void getUpdates();

signals:
    void ErrorCode(quint32 code, const QString &details);
    void Finished(quint32 exit, quint32 runtime);
    void Package(quint32 info, const QString &packageId, const QString &summary);
};
