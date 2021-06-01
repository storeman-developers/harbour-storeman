#pragma once

#include "ornpm.h"
#include "orninstalledpackage.h"
#include "ornpkdaemon.h"
#include "ornpktransaction.h"
#include "ornssu.h"

#include <private/qobject_p.h>
#include <QSet>

#include <functional>

#define PK_FLAG_NONE  quint64(0)

struct s_Pool;

class OrnPmPrivate : public QObjectPrivate
{
    Q_DISABLE_COPY(OrnPmPrivate)
    Q_DECLARE_PUBLIC(OrnPm)

public:
    using ornpm_signal_t = void (OrnPm::*)(const QString &);

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

    void onItemProgress(const QString &id, uint status, uint percentage);

    OrnPkTransaction *packageTransaction(const QString &packageName, OrnPm::Operation operation, OrnPm::PackageStatus status, ornpm_signal_t sgnl);

    // Check for updates
    void getUpdates();
    // Refresh repos
    void refreshNextRepo(quint32 exit, quint32 runtime);

    void processSolvables(bool enabled, std::function<void(const QString&, s_Pool*)> callback) const;

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
