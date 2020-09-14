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

void SsuInterface::addRepo(const QString &alias, const QString &url) {
    QString method{QStringLiteral("addRepo")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << alias << ", " << url << ")";
    callWithArgumentList(QDBus::BlockWithGui, method, QVariantList{alias, url});
}

void SsuInterface::modifyRepo(int action, const QString &alias) {
    QString method{QStringLiteral("modifyRepo")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << action << ", \"" << alias << "\")";
    callWithArgumentList(QDBus::BlockWithGui, method, QVariantList{action, alias});
}
