#pragma once

#include "ornutils.h"

#include <QDBusAbstractInterface>

class OrnPkTransaction : public QDBusAbstractInterface
{
    Q_OBJECT

#ifdef QT_DEBUG
    friend QDebug operator<<(QDebug dbg, const OrnPkTransaction *t);
#endif

public:
    OrnPkTransaction(const QString &path, bool conn, QObject *parent = nullptr);

    void resolve(const QStringList &names);
    void installPackages(const QStringList &ids);
    void updatePackages(const QStringList &ids);
    void removePackages(const QStringList &ids, bool autoremove = false);

    void installFiles(const QStringList &files);

    void repoRefreshNow(const QString &alias, const QString &force);
    void repoRefreshNow(const QString &alias, bool force = false) {
        repoRefreshNow(alias, OrnUtils::stringify(force));
    }

    void refreshCache(bool force = false);    
    void getUpdates();

    Q_PROPERTY(uint Role READ role)
    uint role() const {
        return property("Role").toUInt();
    }

signals:
    void ErrorCode(quint32 code, const QString &details);
    void Finished(quint32 exit, quint32 runtime);
    void Package(quint32 info, const QString &packageId, const QString &summary);
};
