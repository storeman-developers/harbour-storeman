#include "pkinterface.h"
#include "pktransactioninterface.h"

PkInterface::PkInterface(QObject *parent)
    : QDBusAbstractInterface(
          QStringLiteral("org.freedesktop.PackageKit"),
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
        service(),
        qvariant_cast<QDBusObjectPath>(reply.arguments().first()).path(),
        parent()
    );
    Q_ASSERT(t->isValid());

    return t;
}
