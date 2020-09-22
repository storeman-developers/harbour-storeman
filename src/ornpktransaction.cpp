#include "ornpktransaction.h"
#include "ornpkdaemon.h"

#include <Transaction>

#include <QDBusPendingCall>
#include <QDebug>

constexpr quint64 FlagNone{PackageKit::Transaction::TransactionFlagNone};

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
                     << runtime << "msec";
            this->deleteLater();
        });
        connect(this, &OrnPkTransaction::ErrorCode, this, [this](quint32 code, const QString &details) {
            qDebug().noquote().nospace() << this << " error code " << code << ": " << details;
        });
#else
        connect(this, &OrnPkTransaction::Finished, this, &OrnPkTransaction::deleteLater);
#endif
    }
}

void OrnPkTransaction::resolve(const QStringList &names) {
    QString method{QStringLiteral("Resolve")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << FlagNone << ", " << names << ")";
    asyncCall(method, FlagNone, names);
}

void OrnPkTransaction::installPackages(const QStringList &ids) {
    QString method{QStringLiteral("InstallPackages")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << FlagNone << ", " << ids << ")";
    asyncCall(method, FlagNone, ids);
}

void OrnPkTransaction::updatePackages(const QStringList &ids) {
    QString method{QStringLiteral("UpdatePackages")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << FlagNone << ", " << ids << ")";
    asyncCall(method, FlagNone, ids);
}

void OrnPkTransaction::removePackages(const QStringList &ids, bool autoremove) {
    QString method{QStringLiteral("RemovePackages")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << FlagNone << ", " << ids << ", false, " << autoremove << ")";
    asyncCall(method, FlagNone, ids, false, autoremove);
}

void OrnPkTransaction::installFiles(const QStringList &files) {
    QString method{QStringLiteral("InstallFiles")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << FlagNone << ", " << files << ")";
    asyncCall(method, FlagNone, files);
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
            << "Calling " << this << "->" << method << "(" << FlagNone << ")";
    asyncCall(method, FlagNone);
}
