#include "ssuinterface.h"

#include <QDBusPendingCallWatcher>
#include <QDebug>

SsuInterface::SsuInterface(QObject *parent)
    : QDBusAbstractInterface(
          QStringLiteral("org.nemo.ssu"),
          QStringLiteral("/org/nemo/ssu"),
          "org.nemo.ssu",
          QDBusConnection::systemBus(),
          parent
      )
{

}

void waitForFinished(QDBusPendingCallWatcher *watcher) {
    watcher->waitForFinished();
    watcher->deleteLater();
}

QDBusPendingCallWatcher *SsuInterface::addRepoAsync(const QString &alias, const QString &url) {
    QString method{QStringLiteral("addRepo")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << alias << ", " << url << ")";
    return new QDBusPendingCallWatcher(asyncCall(method, alias, url));
}

void SsuInterface::addRepo(const QString &alias, const QString &url) {
    waitForFinished(addRepoAsync(alias, url));
}

QDBusPendingCallWatcher *SsuInterface::modifyRepoAsync(int action, const QString &alias) {
    QString method{QStringLiteral("modifyRepo")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << action << ", \"" << alias << "\")";
    return new QDBusPendingCallWatcher(asyncCall(method, action, alias));
}

void SsuInterface::modifyRepo(int action, const QString &alias) {
    waitForFinished(modifyRepoAsync(action, alias));
}
