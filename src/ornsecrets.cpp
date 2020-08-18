#include "ornsecrets_p.h"
#include "ornsecrets.h"

#include <Sailfish/Secrets/collectionnamesrequest.h>
#include <Sailfish/Secrets/createcollectionrequest.h>
#include <Sailfish/Secrets/request.h>
#include <Sailfish/Secrets/result.h>
#include <Sailfish/Secrets/secret.h>
#include <Sailfish/Secrets/storesecretrequest.h>
#include <Sailfish/Secrets/storedsecretrequest.h>
#include <Sailfish/Secrets/deletecollectionrequest.h>

#include <QDebug>

using namespace Sailfish::Secrets;


static const QString collectionName(QStringLiteral("storeman"));

bool checkResult(const Request &req)
{
    auto result = req.result();
    auto success = result.errorCode() == Result::NoError;
    if (!success) {
        qDebug() << result.errorMessage();
    }
    return success;
}

Secret::Identifier makeIdent(const QString &name)
{
    return Secret::Identifier(name, collectionName, SecretManager::DefaultEncryptedStoragePluginName);
}

bool createCollection(SecretManager *manager)
{
    CreateCollectionRequest ccr;
    ccr.setManager(manager);
    ccr.setCollectionName(collectionName);
    ccr.setAccessControlMode(SecretManager::OwnerOnlyMode);
    ccr.setCollectionLockType(CreateCollectionRequest::DeviceLock);
    ccr.setDeviceLockUnlockSemantic(SecretManager::DeviceLockKeepUnlocked);
    ccr.setStoragePluginName(SecretManager::DefaultEncryptedStoragePluginName);
    ccr.setEncryptionPluginName(SecretManager::DefaultEncryptedStoragePluginName);
    ccr.startRequest();
    ccr.waitForFinished();
    return checkResult(ccr);
}

OrnSecrets::OrnSecrets()
    : d_ptr{new OrnSecretsPrivate()}
{
    CollectionNamesRequest cnr;
    cnr.setManager(d_ptr->secretManager.get());
    cnr.setStoragePluginName(SecretManager::DefaultEncryptedStoragePluginName);
    cnr.startRequest();
    cnr.waitForFinished();
    d_ptr->valid = checkResult(cnr) && cnr.collectionNames().contains(collectionName);
}

OrnSecrets::~OrnSecrets()
{

}

bool OrnSecrets::isValid() const
{
    return d_ptr->valid;
}

bool OrnSecrets::storeData(const QString &name, const QByteArray &data)
{
    if (!d_ptr->valid) {
        d_ptr->valid = createCollection(d_ptr->secretManager.get());
    }

    Secret secret(makeIdent(name));
    secret.setData(data);

    StoreSecretRequest ssr;
    ssr.setManager(d_ptr->secretManager.get());
    ssr.setSecretStorageType(StoreSecretRequest::CollectionSecret);
    ssr.setUserInteractionMode(SecretManager::SystemInteraction);
    ssr.setSecret(secret);
    ssr.startRequest();
    ssr.waitForFinished();

    return checkResult(ssr);
}

QByteArray OrnSecrets::data(const QString &name)
{
    if (!d_ptr->valid) {
        return QByteArray();
    }

    StoredSecretRequest ssr;
    ssr.setManager(d_ptr->secretManager.get());
    ssr.setUserInteractionMode(Sailfish::Secrets::SecretManager::SystemInteraction);
    ssr.setIdentifier(makeIdent(name));;
    ssr.startRequest();
    ssr.waitForFinished();

    auto success = checkResult(ssr);
    if (success)
    {
        return ssr.secret().data();
    }

    return QByteArray();
}

bool OrnSecrets::removeCollection()
{
    if (!d_ptr->valid) {
        return false;
    }

    DeleteCollectionRequest dcr;
    dcr.setManager(d_ptr->secretManager.get());
    dcr.setCollectionName(collectionName);
    dcr.setStoragePluginName(SecretManager::DefaultEncryptedStoragePluginName);
    dcr.setUserInteractionMode(Sailfish::Secrets::SecretManager::SystemInteraction);
    dcr.startRequest();
    dcr.waitForFinished();
    return checkResult(dcr);
}
