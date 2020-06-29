#pragma once

#include <memory>
#include <Sailfish/Secrets/secretmanager.h>

class OrnSecretsPrivate
{
public:
    OrnSecretsPrivate() = default;
    void checkCollection();
    void createCollection();
    bool removeCollection();

    static const QString collectionName;

    using SecretManager = Sailfish::Secrets::SecretManager;

    std::unique_ptr<SecretManager> secretManager{new SecretManager()};
    bool valid{false};
};
