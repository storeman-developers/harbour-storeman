#pragma once

#include <memory>
#include <QByteArray>

struct OrnSecretsPrivate;

class OrnSecrets
{
public:
    OrnSecrets();
    ~OrnSecrets();

    bool isValid() const;

    bool storeData(const QString &name, const QByteArray &data);
    QByteArray data(const QString &name);
    bool removeCollection();

private:
    std::unique_ptr<OrnSecretsPrivate> d_ptr;
};
