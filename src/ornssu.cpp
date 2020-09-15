#include "ornssu.h"

#include <QDBusPendingCallWatcher>
#include <QDebug>

OrnSsu::OrnSsu(QObject *parent)
    : QDBusAbstractInterface(
          QStringLiteral("org.nemo.ssu"),
          QStringLiteral("/org/nemo/ssu"),
          "org.nemo.ssu",
          QDBusConnection::systemBus(),
          parent
      )
{

}

void OrnSsu::addRepo(const QString &alias, const QString &url) {
    QString method{QStringLiteral("addRepo")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << alias << ", " << url << ")";
    callWithArgumentList(QDBus::BlockWithGui, method, QVariantList{alias, url});
}

void OrnSsu::modifyRepo(int action, const QString &alias) {
    QString method{QStringLiteral("modifyRepo")};
    qDebug().nospace()
            << "Calling " << this << "->" << method.toLatin1().data()
            << "(" << action << ", \"" << alias << "\")";
    callWithArgumentList(QDBus::BlockWithGui, method, QVariantList{action, alias});
}
