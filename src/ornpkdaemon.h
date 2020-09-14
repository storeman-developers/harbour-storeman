#pragma once

#include <QDBusAbstractInterface>

class OrnPkTransaction;

class OrnPkDaemon : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    static const QString serviceName;

    explicit OrnPkDaemon(QObject *parent = nullptr);

    OrnPkTransaction *transaction();

signals:
    void UpdatesChanged();
    void TransactionListChanged(const QStringList &transactions);
};
