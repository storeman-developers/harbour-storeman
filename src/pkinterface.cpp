#include "pkinterface.h"
#include "pktransactioninterface.h"

#include <QDebug>

const QString PkInterface::serviceName{QStringLiteral("org.freedesktop.PackageKit")};

PkInterface::PkInterface(QObject *parent)
    : QDBusAbstractInterface(
          serviceName,
          QStringLiteral("/org/freedesktop/PackageKit"),
          "org.freedesktop.PackageKit",
          QDBusConnection::systemBus(),
          parent
      )
{

}

PkTransactionInterface *PkInterface::transaction() {
    auto reply = call(QStringLiteral("CreateTransaction"));
    Q_ASSERT_X(reply.type() != QDBusMessage::ErrorMessage, Q_FUNC_INFO,
               qPrintable(reply.errorName().append(": ").append(reply.errorMessage())));

    auto t = new PkTransactionInterface(
        qvariant_cast<QDBusObjectPath>(reply.arguments().first()).path(),
        false,
        parent()
    );
    Q_ASSERT(t->isValid());

    return t;
}
