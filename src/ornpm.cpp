#include "ornpm_p.h"
#include "ornpackageversion.h"
#include "ornrepo.h"
#include "ornutils.h"

#include <solv/repo_solv.h>
#include <connman-qt5/networkmanager.h>

#include <QtConcurrent/QtConcurrent>

#include <QDebug>

using namespace PackageKit;

#define CHECK_INITIALISED() \
    Q_ASSERT_X(d_ptr->initialised, Q_FUNC_INFO, "Call only after OrnPm was initialised!")

#define CHECK_NETWORK() \
    if (NetworkManager::instance()->state() != QLatin1String("online")) { \
        qWarning("Network is unavailable!"); \
        return; \
    }

#define SET_OPERATION_ITEM(operation, item) \
    CHECK_INITIALISED(); \
    if (d_ptr->operations.contains(item)) { \
        qWarning() << item << "is already being processed!"; \
        return; \
    } \
    d_ptr->operations.insert(item, operation); \
    emit this->operationsChanged();

const QLatin1String OrnPm::repoNamePrefix("openrepos-");

OrnPm *OrnPm::g_instance = nullptr;

OrnPm::OrnPm(QObject *parent)
    : QObject(parent)
    , d_ptr(new OrnPmPrivate(this))
{}

OrnPmPrivate::OrnPmPrivate(OrnPm *ornPm)
    : initialised(false)
    , q_ptr(ornPm)
{
    auto bus = QDBusConnection::systemBus();

    QString service(SSU_SERVICE);
    ssuInterface = new QDBusInterface(service, SSU_PATH, service, bus, q_ptr);

    service = PK_SERVICE;
    pkInterface = new QDBusInterface(service, PK_PATH, service, bus, q_ptr);
    QObject::connect(pkInterface, SIGNAL(UpdatesChanged()), q_ptr, SLOT(getUpdates()));
}

OrnPm::~OrnPm()
{
    delete d_ptr;
}

OrnPm *OrnPm::instance()
{
    if (!g_instance)
    {
        g_instance = new OrnPm(qApp);
        auto fw = new QFutureWatcher<void>(g_instance);
        connect(fw, &QFutureWatcher<void>::finished, [fw]()
        {
            fw->deleteLater();
            OrnPm::instance()->getUpdates();
        });
        fw->setFuture(QtConcurrent::run(g_instance->d_ptr, &OrnPmPrivate::initialise));
    }
    return g_instance;
}

void OrnPmPrivate::initialise()
{
    qDebug() << "Getting the list of ORN repositories";

    // NOTE: A hack for SSU repos. Can break on ssu config changes.
    QSettings ssuSettings(SSU_CONFIG_PATH, QSettings::IniFormat);

    archs << ssuSettings.value(QStringLiteral("arch")).toString()
          << QStringLiteral("noarch");

    auto disabled = ssuSettings.value(SSU_DISABLED_KEY).toStringList().toSet();
    ssuSettings.beginGroup(SSU_REPOS_GROUP);
    auto aliases = ssuSettings.childKeys();

    for (const auto &alias : aliases)
    {
        if (alias.startsWith(OrnPm::repoNamePrefix))
        {
            auto enabled = !disabled.contains(alias);
            qDebug() << "Found" << (enabled ? "enabled" : "disabled") << "repo" << alias;
            repos.insert(alias, enabled);
        }
    }
    qDebug() << "System has" << repos.size() << "ORN repositories";

    qDebug() << "Getting the list of installed packages";
    auto spool = pool_create();
    auto srepo = repo_create(spool, "installed");

    auto sfile = fopen(SOLV_INSTALLED, "r");
    if (!sfile)
    {
        qCritical() << "Could not read " SOLV_INSTALLED;
        repo_free(srepo, 0);
        pool_free(spool);
        return;
    }

    repo_add_solv(srepo, sfile, 0);
    fclose(sfile);

    for (int i = 0; i < spool->nsolvables; ++i)
    {
        auto s = &spool->solvables[i];
        QString name(solvable_lookup_str(s, SOLVABLE_NAME));
        if (name.size() > 0)
        {
            auto id = QStringLiteral("%1;%2;%3;installed").arg(
                        name, solvable_lookup_str(s, SOLVABLE_EVR), solvable_lookup_str(s, SOLVABLE_ARCH));
            installedPackages.insert(name, id);
        }
    }

    repo_free(srepo, 0);
    pool_free(spool);

    qDebug() << installedPackages.size() << "packages are installed";

    qDebug() << "Initialisation finished";
    initialised = true;
    emit q_ptr->initialisedChanged();
}

bool OrnPm::initialised() const
{
    return d_ptr->initialised;
}

QVariantList OrnPm::operations() const
{
    QVariantList res;
    for (auto op = d_ptr->operations.cbegin(); op != d_ptr->operations.cend(); ++op)
    {
        res << QVariantMap{
            { QStringLiteral("item"),      op.key() },
            { QStringLiteral("operation"), op.value() }
        };
    }
    return res;
}

QString OrnPm::deviceModel() const
{
    // Ssu::DeviceModel = 1
    qDebug().nospace() << "Calling " << d_ptr->ssuInterface << "->" SSU_METHOD_DISPLAYNAME "(1)";
    auto call = d_ptr->ssuInterface->call(QDBus::BlockWithGui, QStringLiteral(SSU_METHOD_DISPLAYNAME), 1);
    return call.arguments().first().toString();
}

bool OrnPm::updatesAvailable() const
{
    return d_ptr->updatablePackages.size();
}

QStringList OrnPm::updatablePackages() const
{
    return d_ptr->updatablePackages.keys();
}

QString OrnPm::updateVersion(const QString &packageName) const
{
    if (d_ptr->updatablePackages.contains(packageName))
    {
        return OrnUtils::packageVersion(d_ptr->updatablePackages[packageName]);
    }
    return QString();
}

OrnPm::RepoStatus OrnPm::repoStatus(const QString &alias) const
{
    if (d_ptr->repos.contains(alias))
    {
        return d_ptr->repos[alias] ? RepoEnabled : RepoDisabled;
    }
    return RepoNotInstalled;
}

/*!
    This will never return \l OrnPm::PackageAvailable.
 */
OrnPm::PackageStatus OrnPm::packageStatus(const QString &packageName) const
{
    if (d_ptr->operations.contains(packageName))
    {
        switch (d_ptr->operations[packageName])
        {
        case InstallingPackage:
            return PackageInstalling;
        case RemovingPackage:
            return PackageRemoving;
        case UpdatingPackage:
            return PackageUpdating;
        default:
            Q_UNREACHABLE();
        }
    }

    if (d_ptr->updatablePackages.contains(packageName))
    {
        return PackageUpdateAvailable;
    }
    if (d_ptr->installedPackages.contains(packageName))
    {
        return PackageInstalled;
    }
    return PackageNotInstalled;
}

QDBusInterface *OrnPmPrivate::transaction(const QString &item)
{
    auto reply = pkInterface->call(QStringLiteral("CreateTransaction"));
    Q_ASSERT_X(reply.type() != QDBusMessage::ErrorMessage, Q_FUNC_INFO,
               qPrintable(reply.errorName().append(": ").append(reply.errorMessage())));

    auto t = new QDBusInterface(PK_SERVICE,
                                qvariant_cast<QDBusObjectPath>(reply.arguments().first()).path(),
                                PK_TR_INTERFACE,
                                QDBusConnection::systemBus(),
                                q_ptr);
    Q_ASSERT(t->isValid());
#ifdef QT_DEBUG
    QObject::connect(t, SIGNAL(Finished(quint32,quint32)),  q_ptr, SLOT(onTransactionFinished(quint32,quint32)));
    QObject::connect(t, SIGNAL(ErrorCode(quint32,QString)), q_ptr, SLOT(emitError(quint32,QString)));
#else
    QObject::connect(t, SIGNAL(Finished(quint32, quint32)), t, SLOT(deleteLater()));
    QObject::connect(t, SIGNAL(ErrorCode(quint32,QString)), q_ptr, SIGNAL(error(quint32,QString)));
#endif
    if (!item.isEmpty())
    {
        transactionHash.insert(t, item);
    }
    return t;
}

#ifdef QT_DEBUG
void OrnPm::onTransactionFinished(quint32 exit, quint32 runtime)
{
    qDebug() << this->sender()
             << (exit == Transaction::ExitSuccess ? "finished in" : "failed after")
             << runtime << "msec";
    this->sender()->deleteLater();
}

void OrnPm::emitError(quint32 code, const QString& details)
{
    qDebug() << this->sender() << "error code" << code << "-" << details;
    emit this->error(code, details);
}
#endif

void OrnPm::getUpdates()
{
    CHECK_NETWORK();
    auto t = d_ptr->transaction();
    connect(t, SIGNAL(Package(quint32,QString,QString)), this, SLOT(onPackageUpdate(quint32,QString,QString)));
    connect(t, SIGNAL(Finished(quint32,quint32)), this, SLOT(onGetUpdatesFinished(quint32,quint32)));
    qDebug().nospace() << "Calling " << t << "->" PK_METHOD_GETUPDATES "(" << PK_FLAG_NONE << ")";
    t->asyncCall(QStringLiteral(PK_METHOD_GETUPDATES), PK_FLAG_NONE);
}

void OrnPm::onPackageUpdate(quint32 info, const QString& packageId, const QString &summary)
{
    Q_UNUSED(summary)
    Q_ASSERT(info == Transaction::InfoEnhancement);
    // Filter updates only for ORN packages
    if (OrnUtils::packageRepo(packageId).startsWith(repoNamePrefix))
    {
        d_ptr->newUpdatablePackages.insert(OrnUtils::packageName(packageId), packageId);
    }
}

void OrnPm::onGetUpdatesFinished(quint32 status, quint32 runtime)
{
    Q_UNUSED(runtime)
    if (status == Transaction::ExitSuccess)
    {
        bool newupdates = false;
        // If some client listen to packageStatusChanged() and want to take a package
        // update ID we swap two hashes before the checks
        d_ptr->updatablePackages.swap(d_ptr->newUpdatablePackages);
        auto it = d_ptr->updatablePackages.begin();
        while (it != d_ptr->updatablePackages.end())
        {
            auto &name = it.key();
            auto &id   = it.value();
            auto repo  = OrnUtils::packageRepo(id);
            // A walkaround to skip inactual updates from removed/disabled repos
            if (!d_ptr->repos.contains(repo) || !d_ptr->repos[repo])
            {
                it = d_ptr->updatablePackages.erase(it);
            }
            else
            {
                // Check if the update is really new
                if (!d_ptr->newUpdatablePackages.contains(name) ||
                    d_ptr->newUpdatablePackages[name] != id)
                {
                    emit this->packageStatusChanged(name, OrnPm::PackageUpdateAvailable);
                    newupdates = true;
                }
                ++it;
            }
        }
        if (newupdates ||
            (d_ptr->updatablePackages.size() != d_ptr->newUpdatablePackages.size()))
        {
            emit this->updatablePackagesChanged();
        }
    }
    d_ptr->newUpdatablePackages.clear();
}

void OrnPm::getPackageVersions(const QString &packageName)
{
    Q_ASSERT(!packageName.isEmpty());
    CHECK_INITIALISED();
    qDebug() << "Resolving package versions for" << packageName;

    QtConcurrent::run(d_ptr, &OrnPmPrivate::preparePackageVersions, packageName);
}

void OrnPmPrivate::preparePackageVersions(const QString &packageName)
{
    OrnPackageVersionList versions;
    auto spool = pool_create();
    auto srepo = repo_create(spool, "");

    auto sfile = fopen(SOLV_INSTALLED, "r");
    if (!sfile)
    {
        qCritical() << "Could not read " SOLV_INSTALLED;
        repo_free(srepo, 0);
        pool_free(spool);
        return;
    }
    repo_add_solv(srepo, sfile, 0);
    fclose(sfile);
    for (int i = 0; i < spool->nsolvables && versions.isEmpty(); ++i)
    {
        auto s = &spool->solvables[i];
        QString name(solvable_lookup_str(s, SOLVABLE_NAME));
        if (name == packageName)
        {
            versions << OrnPackageVersion(
                            0,
                            solvable_lookup_num(s, SOLVABLE_INSTALLSIZE, 0),
                            solvable_lookup_str(s, SOLVABLE_EVR),
                            solvable_lookup_str(s, SOLVABLE_ARCH),
                            QStringLiteral("installed"));
        }
    }
    repo_free(srepo, 0);

    QString solvTmpl(SOLV_PATH_TMPL);
    for (auto it = repos.cbegin(); it != repos.cend(); ++it)
    {
        if (it.value())
        {
            auto alias = it.key();
            auto spath = solvTmpl.arg(alias);
            srepo = repo_create(spool, "");

            qDebug() << "Reading" << spath;
            auto sfile = fopen(spath.toUtf8().data(), "r");
            if (!sfile)
            {
                qCritical() << "Could not read" << spath;
                repo_free(srepo, 0);
                continue;
            }

            repo_add_solv(srepo, sfile, 0);
            fclose(sfile);
            for (int i = 0; i < spool->nsolvables; ++i)
            {
                auto s = &spool->solvables[i];
                if (packageName == solvable_lookup_str(s, SOLVABLE_NAME))
                {
                    QString arch = solvable_lookup_str(s, SOLVABLE_ARCH);
                    if (archs.contains(arch))
                    {
                        versions << OrnPackageVersion(
                                        solvable_lookup_num(s, SOLVABLE_DOWNLOADSIZE, 0),
                                        solvable_lookup_num(s, SOLVABLE_INSTALLSIZE, 0),
                                        solvable_lookup_str(s, SOLVABLE_EVR),
                                        arch,
                                        alias);
                    }
                }
            }
            repo_free(srepo, 0);
        }
    }

    pool_free(spool);
    std::sort(versions.rbegin(), versions.rend());

    qDebug() << "Finished resolving versions for package" << packageName;
    emit q_ptr->packageVersions(packageName, versions);
}

void OrnPm::installPackage(const QString &packageId)
{
    CHECK_NETWORK();
    SET_OPERATION_ITEM(InstallingPackage, OrnUtils::packageName(packageId));

    auto t = d_ptr->transaction(packageId);
    connect(t, SIGNAL(Finished(quint32,quint32)), this, SLOT(onPackageInstalled(quint32,quint32)));
    QStringList ids(packageId);
    qDebug().nospace() << "Calling " << t << "->" PK_METHOD_INSTALLPACKAGES "(" << PK_FLAG_NONE << ", " << ids << ")";
    emit this->packageStatusChanged(OrnUtils::packageName(packageId), OrnPm::PackageInstalling);
    t->asyncCall(QStringLiteral(PK_METHOD_INSTALLPACKAGES), PK_FLAG_NONE, ids);
}

void OrnPm::installFile(const QString &packageFile)
{
    CHECK_NETWORK();

    auto t = d_ptr->transaction();
    connect(t, SIGNAL(Finished(quint32,quint32)), this, SLOT(onPackageInstalled(quint32,quint32)));
    QStringList files(packageFile);
    qDebug().nospace() << "Calling " << t << "->" PK_METHOD_INSTALLFILES "(" << PK_FLAG_NONE << ", " << files << ")";
    t->asyncCall(QStringLiteral(PK_METHOD_INSTALLFILES), PK_FLAG_NONE, files);
}

void OrnPm::onPackageInstalled(quint32 exit, quint32 runtime)
{
    Q_UNUSED(runtime)
    auto id = d_ptr->transactionHash.take(this->sender());
    auto name = OrnUtils::packageName(id);
    d_ptr->operations.remove(name);
    emit this->operationsChanged();
    if (exit == Transaction::ExitSuccess)
    {
        d_ptr->installedPackages[name] = id;
        emit this->packageInstalled(name);
        emit this->packageStatusChanged(name, OrnPm::PackageInstalled);
    }
    else
    {
        emit this->packageStatusChanged(name, OrnPm::PackageUnknownStatus);
    }
}

void OrnPm::removePackage(const QString &packageId, bool autoremove)
{
    SET_OPERATION_ITEM(RemovingPackage, OrnUtils::packageName(packageId));

    auto t = d_ptr->transaction(packageId);
    connect(t, SIGNAL(Finished(quint32,quint32)), this, SLOT(onPackageRemoved(quint32,quint32)));
    QStringList ids(packageId);
    qDebug().nospace() << "Calling " << t << "->" PK_METHOD_REMOVEPACKAGES "("
                       << PK_FLAG_NONE << ", " << ids << ", false, " << autoremove << ")";
    emit this->packageStatusChanged(OrnUtils::packageName(packageId), OrnPm::PackageRemoving);
    t->asyncCall(QStringLiteral(PK_METHOD_REMOVEPACKAGES), PK_FLAG_NONE, ids, false, autoremove);
}

void OrnPm::onPackageRemoved(quint32 exit, quint32 runtime)
{    
    Q_UNUSED(runtime)
    auto id = d_ptr->transactionHash.take(this->sender());
    auto name = OrnUtils::packageName(id);
    d_ptr->operations.remove(name);
    emit this->operationsChanged();
    if (exit == Transaction::ExitSuccess)
    {
        d_ptr->installedPackages.remove(name);
        emit this->packageRemoved(name);
        emit this->packageStatusChanged(name, OrnPm::PackageNotInstalled);
    }
    else
    {
        emit this->packageStatusChanged(name, OrnPm::PackageUnknownStatus);
    }
}

void OrnPm::updatePackage(const QString &packageName)
{
    if (!d_ptr->updatablePackages.contains(packageName))
    {
        qWarning() << "The package" << packageName << "has no updates!";
        return;
    }
    CHECK_NETWORK();
    SET_OPERATION_ITEM(UpdatingPackage, packageName);

    auto packageId = d_ptr->updatablePackages[packageName];
    auto t = d_ptr->transaction(packageId);
    connect(t, SIGNAL(Finished(quint32,quint32)), this, SLOT(onPackageUpdated(quint32,quint32)));
    QStringList ids(packageId);
    qDebug().nospace() << "Calling " << t << "->" PK_METHOD_UPDATEPACKAGES "(" << PK_FLAG_NONE << ", " << ids << ")";
    emit this->packageStatusChanged(packageName, OrnPm::PackageUpdating);
    t->asyncCall(QStringLiteral(PK_METHOD_UPDATEPACKAGES), PK_FLAG_NONE, ids);
}

void OrnPm::onPackageUpdated(quint32 exit, quint32 runtime)
{
    Q_UNUSED(runtime)
    auto id = d_ptr->transactionHash.take(this->sender());
    auto name = OrnUtils::packageName(id);
    d_ptr->operations.remove(name);
    emit this->operationsChanged();
    if (exit == Transaction::ExitSuccess)
    {
        d_ptr->updatablePackages.remove(name);
        d_ptr->installedPackages[name] = id;
        emit this->packageUpdated(name);
        emit this->packageStatusChanged(name, OrnPm::PackageInstalled);
        emit this->updatablePackagesChanged();
    }
    else
    {
        emit this->packageStatusChanged(name, OrnPm::PackageUnknownStatus);
    }
}

void OrnPm::addRepo(const QString &author)
{
    CHECK_NETWORK();
    CHECK_INITIALISED();

    auto repoAlias = repoNamePrefix + author;
    SET_OPERATION_ITEM(AddingRepo, repoAlias);
    auto url = REPO_URL_TMPL.arg(author);
    qDebug().nospace() << "Calling " << d_ptr->ssuInterface << "->" SSU_METHOD_ADDREPO "("
                       << repoAlias << ", " << url << ")";
    auto watcher = new QDBusPendingCallWatcher(
                d_ptr->ssuInterface->asyncCall(QStringLiteral(SSU_METHOD_ADDREPO), repoAlias, url));
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher, repoAlias]()
    {
        d_ptr->onRepoModified(repoAlias, AddRepo);
        watcher->deleteLater();
    });
}

void OrnPm::modifyRepo(const QString &repoAlias, const OrnPm::RepoAction &action)
{
    CHECK_NETWORK();
    Operation op;
    switch (action)
    {
    case RemoveRepo:
        op = RemovingRepo;
        break;
    case DisableRepo:
        op = DisablingRepo;
        break;
    case EnableRepo:
        op = EnablingRepo;
        break;
    default:
        Q_UNREACHABLE();
    }
    SET_OPERATION_ITEM(op, repoAlias);

    qDebug().nospace() << "Calling " << d_ptr->ssuInterface << "->" SSU_METHOD_MODIFYREPO "("
                       << action << ", " << repoAlias << ")";
    auto watcher = new QDBusPendingCallWatcher(
                d_ptr->ssuInterface->asyncCall(QStringLiteral(SSU_METHOD_MODIFYREPO), action, repoAlias));
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher, repoAlias, action]()
    {
        d_ptr->onRepoModified(repoAlias, action);
        watcher->deleteLater();
    });
}

void OrnPm::enableRepos(bool enable)
{
    CHECK_INITIALISED();

    QtConcurrent::run(d_ptr, &OrnPmPrivate::enableRepos, enable);
}

void OrnPm::removeAllRepos()
{
    QtConcurrent::run(d_ptr, &OrnPmPrivate::removeAllRepos);
}

void OrnPmPrivate::enableRepos(bool enable)
{
    CHECK_NETWORK();
    qDebug() << (enable ? "Enabling" : "Disabling") << "all repositories";
    QString method(QStringLiteral(SSU_METHOD_MODIFYREPO));

    if (enable)
    {
        bool needRefresh = false;
        for (auto it = repos.begin(); it != repos.end(); ++it)
        {
            if (!it.value())
            {
                ssuInterface->call(method, OrnPm::EnableRepo, it.key());
                *it = true;
                needRefresh = true;
            }
        }
        if (needRefresh)
        {
            q_ptr->refreshRepos();
        }
    }
    else
    {
        for (auto it = repos.begin(); it != repos.end(); ++it)
        {
            if (it.value())
            {
                ssuInterface->call(method, OrnPm::DisableRepo, it.key());
                *it = false;
            }
        }
        updatablePackages.clear();
        emit q_ptr->updatablePackagesChanged();
    }

    qDebug() << "Finished" << (enable ? "enabling" : "disabling") << "all repositories";
    emit q_ptr->enableReposFinished();
}

void OrnPmPrivate::removeAllRepos()
{
    qDebug() <<"Removing all repositories";
    QString method(QStringLiteral(SSU_METHOD_MODIFYREPO));

    for (auto it = repos.begin(); it != repos.end(); ++it)
    {
        ssuInterface->call(method, OrnPm::RemoveRepo, it.key());
    }
    repos.clear();
    updatablePackages.clear();
    emit q_ptr->updatablePackagesChanged();

    qDebug() << "Finished removing all repositories";
    emit q_ptr->removeAllReposFinished();
}

void OrnPmPrivate::onRepoModified(const QString &repoAlias, const OrnPm::RepoAction &action)
{
    bool needRefresh = false;

    switch (action)
    {
    case OrnPm::RemoveRepo:
        repos.remove(repoAlias);
        break;
    case OrnPm::AddRepo:
        repos.insert(repoAlias, true);
        needRefresh = true;
        break;
    case OrnPm::DisableRepo:
        repos[repoAlias] = false;
        break;
    case OrnPm::EnableRepo:
        repos[repoAlias] = true;
        needRefresh = true;
        break;
    }

    if (needRefresh)
    {
        operations[repoAlias] = OrnPm::RefreshingRepo;
        emit q_ptr->operationsChanged();
        auto t = this->transaction();
        qDebug().nospace() << "Calling " << t << "->" PK_METHOD_REPOSETDATA "("
                           << repoAlias << ", \"refresh-now\", false)";
        t->asyncCall(QStringLiteral(PK_METHOD_REPOSETDATA), repoAlias,
                     QStringLiteral("refresh-now"), QStringLiteral("false"));
        QObject::connect(t, &QDBusInterface::destroyed, [this, repoAlias, action]()
        {
            operations.remove(repoAlias);
            emit q_ptr->operationsChanged();
            emit q_ptr->repoModified(repoAlias, action);
            qDebug() << "Repo" << repoAlias << "have been modified with" << action;
        });
    }
    else
    {
        operations.remove(repoAlias);
        emit q_ptr->operationsChanged();
        emit q_ptr->repoModified(repoAlias, action);
        qDebug() << "Repo" << repoAlias << "have been modified with" << action;
        q_ptr->getUpdates();
    }
}

void OrnPm::refreshRepo(const QString &repoAlias, bool force)
{
    CHECK_NETWORK();
    SET_OPERATION_ITEM(RefreshingRepo, repoAlias);
    auto t = d_ptr->transaction();
    connect(t, &QDBusInterface::destroyed, [this, repoAlias]()
    {
        d_ptr->operations.remove(repoAlias);
        emit this->operationsChanged();
    });
    qDebug().nospace() << "Calling " << t << "->RepoSetData(" << repoAlias
                       << ", \"refresh-now\", " << (force ? "true" : "false") << ")";
    t->asyncCall(QStringLiteral("RepoSetData"), repoAlias, QStringLiteral("refresh-now"),
                 force ? QStringLiteral("true") : QStringLiteral("false"));
}

void OrnPm::refreshRepos(bool force)
{
    CHECK_NETWORK();

    for (auto it = d_ptr->repos.cbegin(); it != d_ptr->repos.cend(); ++it)
    {
        // Refresh only enabled repositories
        if (it.value())
        {
            d_ptr->reposToRefresh << it.key();
        }
    }

    if (d_ptr->reposToRefresh.isEmpty())
    {
        qDebug() << "No enabled repositories, skipping";
        return;
    }

    qDebug() << "Starting refresh cache for all ORN repositories";

    d_ptr->pkInterface->blockSignals(true);
    d_ptr->forceRefresh = force ? QStringLiteral("true") : QStringLiteral("false");
#ifdef QT_DEBUG
    d_ptr->refreshRuntime = 0;
#endif
    this->refreshNextRepo(Transaction::ExitSuccess, 0);
}

void OrnPm::refreshNextRepo(quint32 exit, quint32 runtime)
{
    Q_UNUSED(exit)

#ifdef QT_DEBUG
    d_ptr->refreshRuntime += runtime;
#else
    Q_UNUSED(runtime)
#endif

    bool offline = NetworkManager::instance()->state() != QLatin1String("online");
    if (offline)
    {
        d_ptr->reposToRefresh.clear();
    }
    if (d_ptr->reposToRefresh.isEmpty())
    {
#ifdef QT_DEBUG
        if (offline)
        {
            qWarning("Aborting operation due to network gone offline");
        }
        else
        {
            qDebug() << "Finished refreshing cache for all ORN repositories in"
                     << d_ptr->refreshRuntime << "msec";
        }
#endif
        d_ptr->pkInterface->blockSignals(false);
    }
    else
    {
        auto t = d_ptr->transaction();
        connect(t, SIGNAL(Finished(quint32,quint32)), this, SLOT(refreshNextRepo(quint32,quint32)));
        auto alias = d_ptr->reposToRefresh.takeFirst();
        connect(t, &QDBusInterface::destroyed, [this, alias]()
        {
            d_ptr->operations.remove(alias);
            emit this->operationsChanged();
        });
        d_ptr->operations.insert(alias, RefreshingRepo);
        emit this->operationsChanged();
        qDebug().nospace() << "Calling " << t << "->" PK_METHOD_REPOSETDATA "("
                           << alias << ", \"refresh-now\", " << d_ptr->forceRefresh << ")";
        t->asyncCall(QStringLiteral(PK_METHOD_REPOSETDATA), alias, QStringLiteral("refresh-now"),
                     d_ptr->forceRefresh);
    }
}

QList<OrnRepo> OrnPm::repoList() const
{
    OrnRepoList repos;
    auto pos = repoNamePrefix.size();
    for (auto it = d_ptr->repos.cbegin(); it != d_ptr->repos.cend(); ++it)
    {
        auto alias = it.key();
        repos << OrnRepo{ it.value(), alias, alias.mid(pos) };
    }
    return repos;
}

void OrnPm::getInstalledPackages(const QString &packageName)
{
    auto watcher = new QFutureWatcher<OrnInstalledPackageList>();
    connect(watcher, &QFutureWatcher<OrnInstalledPackageList>::finished, [this, watcher]()
    {
        auto res = watcher->result();
        if (!res.isEmpty())
        {
            emit this->installedPackages(res);
        }
        watcher->deleteLater();
    });
    watcher->setFuture(QtConcurrent::run(d_ptr, &OrnPmPrivate::prepareInstalledPackages, packageName));
}

OrnInstalledPackageList OrnPmPrivate::prepareInstalledPackages(const QString &packageName)
{
    Q_ASSERT_X(packageName.isEmpty() || installedPackages.contains(packageName), Q_FUNC_INFO,
               qPrintable(QString("The provided package \"%0\" is not installed").arg(packageName)));

    OrnInstalledPackageList packages;

    if (installedPackages.isEmpty() || repos.isEmpty())
    {
        qWarning() << "Installed packages or repositories list is empty";
        emit q_ptr->installedPackages(packages);
        return packages;
    }
    qDebug() << "Preparing installed packages list";

    // Prepare vars for parsing desktop files
    QString nameKey(QStringLiteral("Desktop Entry/Name"));
    auto trNameKey = QString(nameKey).append("[%0]");
    auto localeName = QLocale::system().name();
    auto localeNameKey = trNameKey.arg(localeName);
    QString langNameKey;
    if (localeName.length() > 2)
    {
        langNameKey = trNameKey.arg(localeName.left(2));
    }
    QString iconKey(QStringLiteral("Desktop Entry/Icon"));
    QStringList iconPaths = {
        QStringLiteral("/usr/share/icons/hicolor/86x86/apps/%0.png"),
        QStringLiteral("/usr/share/icons/hicolor/108x108/apps/%0.png"),
        QStringLiteral("/usr/share/icons/hicolor/128x128/apps/%0.png"),
        QStringLiteral("/usr/share/icons/hicolor/256x256/apps/%0.png")
    };

    // Prepare set to filter installed packages to show only those from OpenRepos
    QString solvTmpl(SOLV_PATH_TMPL);
    StringSet ornPackages;
    auto spool = pool_create();
    for (auto it = repos.cbegin(); it != repos.cend(); ++it)
    {
        if (it.value())
        {
            const auto alias = it.key();
            auto spath = solvTmpl.arg(alias);
            qDebug() << "Reading" << spath;
            auto srepo = repo_create(spool, alias.toUtf8().data());

            auto sfile = fopen(spath.toUtf8().data(), "r");
            if (!sfile)
            {
                qCritical() << "Could not read" << spath;
                repo_free(srepo, 0);
                continue;
            }

            repo_add_solv(srepo, sfile, 0);
            for (int i = 0; i < spool->nsolvables; ++i)
            {
                auto s = &spool->solvables[i];
                ornPackages.insert(solvable_lookup_str(s, SOLVABLE_NAME));
            }
            fclose(sfile);
            repo_free(srepo, 0);
        }
    }
    pool_free(spool);

    StringHash installed;
    if (packageName.isEmpty())
    {
        installed = installedPackages;
    }
    else
    {
        installed[packageName] = installedPackages[packageName];
    }

    for (auto it = installed.cbegin(); it != installed.cend(); ++it)
    {
        const auto &name = it.key();

        // Actual filtering
        if (!ornPackages.contains(name))
        {
            continue;
        }

        qDebug() << "Adding installed package" << name;

        auto title = name;
        QString icon;
        auto desktopFile = QStandardPaths::locate(
                    QStandardPaths::ApplicationsLocation, name + ".desktop");

        if (!desktopFile.isEmpty())
        {
            qDebug() << "Parsing desktop file" << desktopFile;
            QSettings desktop(desktopFile, QSettings::IniFormat);
            desktop.setIniCodec("UTF-8");
            // Read pretty name
            if (desktop.contains(localeNameKey))
            {
                title = desktop.value(localeNameKey).toString();
            }
            else if (!langNameKey.isEmpty() && desktop.contains(langNameKey))
            {
                title = desktop.value(langNameKey).toString();
            }
            else if (desktop.contains(nameKey))
            {
                title = desktop.value(nameKey).toString();
            }
            qDebug() << "Using name" << title << "for package" << name;
            // Find icon
            if (desktop.contains(iconKey))
            {
                auto iconName = desktop.value(iconKey).toString();
                for (const auto &path : iconPaths)
                {
                    auto iconPath = path.arg(iconName);
                    if (QFileInfo(iconPath).isFile())
                    {
                        qDebug() << "Using package icon" << iconPath;
                        icon = iconPath;
                        break;
                    }
                }
            }
        }
        auto id = it.value();
        packages << OrnInstalledPackage {
            updatablePackages.contains(name),
            id,
            OrnUtils::packageName(id),
            title,
            icon
        };
    }

    return packages;
}
