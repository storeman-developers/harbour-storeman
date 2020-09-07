#pragma once

#include <QDBusAbstractInterface>

class PkTransactionInterface;

class PkInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    explicit PkInterface(QObject *parent = nullptr);

    PkTransactionInterface *transaction();

signals:
    void UpdatesChanged();
};
