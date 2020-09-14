#pragma once

#include <QDBusAbstractInterface>

class PkTransactionInterface;

class PkInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    static const QString serviceName;

    explicit PkInterface(QObject *parent = nullptr);

    PkTransactionInterface *transaction();

signals:
    void UpdatesChanged();
    void TransactionListChanged(const QStringList &transactions);
};
