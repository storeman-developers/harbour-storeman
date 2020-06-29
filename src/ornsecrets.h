#pragma once

#include <memory>
#include <QByteArray>

class OrnSecretsPrivate;

class OrnSecrets
{
public:
    OrnSecrets();
    ~OrnSecrets();

    bool isValid() const;

    bool storeData(const QString &name, const QByteArray &data);
    QByteArray data(const QString &name);
    void reset();

private:
    std::unique_ptr<OrnSecretsPrivate> d_ptr;
};
