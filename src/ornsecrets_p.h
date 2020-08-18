#pragma once

#include <memory>
#include <Sailfish/Secrets/secretmanager.h>

struct OrnSecretsPrivate
{
    using SecretManager = Sailfish::Secrets::SecretManager;

    std::unique_ptr<SecretManager> secretManager{new SecretManager()};
    bool valid{false};
};
