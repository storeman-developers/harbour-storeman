#include "ornpkdaemon.h"
#include "ornpktransaction.h"

#include <QDebug>

const QString OrnPkDaemon::serviceName{QStringLiteral("org.freedesktop.PackageKit")};

OrnPkDaemon::OrnPkDaemon(QObject *parent)
    : QDBusAbstractInterface(
          serviceName,
          QStringLiteral("/org/freedesktop/PackageKit"),
          "org.freedesktop.PackageKit",
          QDBusConnection::systemBus(),
          parent
      )
{

}

OrnPkTransaction *OrnPkDaemon::transaction() {
    auto reply = call(QStringLiteral("CreateTransaction"));
    Q_ASSERT_X(reply.type() != QDBusMessage::ErrorMessage, Q_FUNC_INFO,
               qPrintable(reply.errorName().append(": ").append(reply.errorMessage())));

    auto t = new OrnPkTransaction(
        qvariant_cast<QDBusObjectPath>(reply.arguments().constFirst()).path(),
        false,
        parent()
    );
    Q_ASSERT(t->isValid());

    return t;
}
