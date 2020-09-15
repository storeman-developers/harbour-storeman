#pragma once

#include "ornpm.h"
#include "orninstalledpackage.h"
#include "ornpkdaemon.h"
#include "ornpktransaction.h"
#include "ornssu.h"

#include <private/qobject_p.h>
#include <QSet>


#define PK_FLAG_NONE  quint64(0)

class OrnPmPrivate : public QObjectPrivate
{
    Q_DISABLE_COPY(OrnPmPrivate)
    Q_DECLARE_PUBLIC(OrnPm)

public:
    OrnPmPrivate() = default;
    ~OrnPmPrivate() override = default;

    void initialise();
    OrnPkTransaction *transaction();
    OrnPkTransaction *currentTransaction();
    void preparePackageVersions(const QString &packageName);
    bool enableRepos(bool enable);
    void removeAllRepos();
    void onRepoModified(const QString &alias, OrnPm::RepoAction action);
    OrnInstalledPackageList prepareInstalledPackages(const QString &packageName);

    bool startOperation(const QString &name, OrnPm::Operation operation);
    void finishOperation(const QString &name);

    void onPackage(quint32 info, const QString& packageId, const QString &summary);
    void onTransactionFinished(quint32 status, quint32 runtime);

    // Check for updates
    void getUpdates();
    void onPackageUpdate(quint32 info, const QString& packageId);
    void onGetUpdatesFinished(quint32 status);
    // Install package
    void onPackageInstalled(const QString& packageId);
    // Remove package
    void onPackageRemoved(const QString& packageId);
    // Update package
    void onPackageUpdated(const QString& packageId);
    // Refresh repos
    void refreshNextRepo(quint32 exit, quint32 runtime);

    // <alias, enabled>
    using RepoHash      = QHash<QString, bool>;
    using StringSet     = QSet<QString>;
    using StringHash    = QHash<QString, QString>;
    using OperationHash = QHash<QString, OrnPm::Operation>;

    bool            initialised{false};
#ifdef QT_DEBUG
    quint64         refreshRuntime{0};
#endif
    QString         solvPathTmpl;
    StringSet       archs;
    OrnSsu          *ssuInterface{nullptr};
    OrnPkDaemon     *pkDaemon{nullptr};
    RepoHash        repos;
    StringHash      installedPackages;
    StringHash      updatablePackages;
    StringHash      newUpdatablePackages;
    OperationHash   operations;
    QStringList     reposToRefresh;
    QString         forceRefresh;
};
