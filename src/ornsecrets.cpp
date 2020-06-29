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


const QString OrnSecretsPrivate::collectionName(QStringLiteral("storeman"));

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
    return Secret::Identifier(name, OrnSecretsPrivate::collectionName, SecretManager::DefaultEncryptedStoragePluginName);
}

void OrnSecretsPrivate::checkCollection()
{
    CollectionNamesRequest cnr;
    cnr.setManager(secretManager.get());
    cnr.setStoragePluginName(SecretManager::DefaultEncryptedStoragePluginName);
    cnr.startRequest();
    cnr.waitForFinished();
    valid = checkResult(cnr) && cnr.collectionNames().contains(collectionName);
}

void OrnSecretsPrivate::createCollection()
{
    CreateCollectionRequest ccr;
    ccr.setManager(secretManager.get());
    ccr.setCollectionName(collectionName);
    ccr.setAccessControlMode(SecretManager::OwnerOnlyMode);
    ccr.setCollectionLockType(CreateCollectionRequest::DeviceLock);
    ccr.setDeviceLockUnlockSemantic(SecretManager::DeviceLockKeepUnlocked);
    ccr.setStoragePluginName(SecretManager::DefaultEncryptedStoragePluginName);
    ccr.setEncryptionPluginName(SecretManager::DefaultEncryptedStoragePluginName);
    ccr.startRequest();
    ccr.waitForFinished();
    valid = checkResult(ccr);
}

bool OrnSecretsPrivate::removeCollection()
{
    DeleteCollectionRequest dcr;
    dcr.setManager(secretManager.get());
    dcr.setCollectionName(collectionName);
    dcr.setStoragePluginName(SecretManager::DefaultEncryptedStoragePluginName);
    dcr.setUserInteractionMode(Sailfish::Secrets::SecretManager::SystemInteraction);
    dcr.startRequest();
    dcr.waitForFinished();
    return checkResult(dcr);
}

OrnSecrets::OrnSecrets()
    : d_ptr{new OrnSecretsPrivate()}
{
    d_ptr->checkCollection();
    if (!d_ptr->valid)
    {
        d_ptr->createCollection();
    }
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
    Q_ASSERT(d_ptr->valid);

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
    Q_ASSERT(d_ptr->valid);

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

void OrnSecrets::reset()
{
    Q_ASSERT(d_ptr->valid);

    if (d_ptr->removeCollection())
    {
        d_ptr->createCollection();
    }
}
