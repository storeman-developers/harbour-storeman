#include "ornpktransaction.h"
#include "ornpkdaemon.h"

#include <Transaction>

#include <QDBusPendingCall>
#include <QDebug>

static constexpr quint64 PK_FLAG_NONE{0};

#ifdef QT_DEBUG
QDebug operator<<(QDebug dbg, const OrnPkTransaction *t) {
    QDebugStateSaver state{dbg};
    dbg.quote().nospace() << "PackageKit::Transaction(" << t->path() << ")";
    return dbg;
}
#endif

OrnPkTransaction::OrnPkTransaction(const QString &path, bool conn, QObject *parent)
    : QDBusAbstractInterface(
          OrnPkDaemon::serviceName,
          path,
          "org.freedesktop.PackageKit.Transaction",
          QDBusConnection::systemBus(),
          parent
      )
{
    if (conn) {
#ifdef QT_DEBUG
        connect(this, &OrnPkTransaction::Finished, this, [this](quint32 exit, quint32 runtime) {
            qDebug() << this
                     << (exit == PackageKit::Transaction::ExitSuccess ? "finished in" : "failed after")
                     << runtime << " msec";
            this->deleteLater();
        });
        connect(this, &OrnPkTransaction::ErrorCode, this, [this](quint32 code, const QString &details) {
            qDebug().noquote().nospace() << this << " error code " << code << ": " << details;
        });
#else
        connect(this, &PkTransactionInterface::Finished, this, &PkTransactionInterface::deleteLater);
#endif
    }
}

void OrnPkTransaction::resolve(const QStringList &names) {
    QString method{QStringLiteral("Resolve")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << PK_FLAG_NONE << ", " << names << ")";
    asyncCall(method, PK_FLAG_NONE, names);
}

void OrnPkTransaction::installPackages(const QStringList &ids) {
    QString method{QStringLiteral("InstallPackages")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << PK_FLAG_NONE << ", " << ids << ")";
    asyncCall(method, PK_FLAG_NONE, ids);
}

void OrnPkTransaction::updatePackages(const QStringList &ids) {
    QString method{QStringLiteral("UpdatePackages")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << PK_FLAG_NONE << ", " << ids << ")";
    asyncCall(method, PK_FLAG_NONE, ids);
}

void OrnPkTransaction::removePackages(const QStringList &ids, bool autoremove) {
    QString method{QStringLiteral("RemovePackages")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << PK_FLAG_NONE << ", " << ids << ", false, " << autoremove << ")";
    asyncCall(method, PK_FLAG_NONE, ids, false, autoremove);
}

void OrnPkTransaction::installFiles(const QStringList &files) {
    QString method{QStringLiteral("InstallFiles")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << PK_FLAG_NONE << ", " << files << ")";
    asyncCall(method, PK_FLAG_NONE, files);
}

void OrnPkTransaction::repoRefreshNow(const QString &alias, const QString &force) {
    QString method{QStringLiteral("RepoSetData")};
    qDebug().nospace().noquote()
            << "Calling " << this << "->" << method
            << "(\"" << alias << "\", \"refresh-now\", " << force << ")";
    asyncCall(method, alias, "refresh-now", force);
}

void OrnPkTransaction::refreshCache(bool force) {
    QString method{QStringLiteral("RefreshCache")};
    qDebug().nospace().noquote()
            << "Calling " << this << "->" << method << "(" << force << ")";
    asyncCall(method, force);
}

void OrnPkTransaction::getUpdates() {
    QString method{QStringLiteral("GetUpdates")};
    qDebug().nospace().noquote()
            << "Calling " << this << "->" << method << "(" << PK_FLAG_NONE << ")";
    asyncCall(method, PK_FLAG_NONE);
}
