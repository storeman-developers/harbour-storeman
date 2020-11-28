#include "ornpm_p.h"
#include "ornpackageversion.h"
#include "ornrepo.h"
#include "ornutils.h"
#include "ornconst.h"

#include <solv/repo_solv.h>
#include <connman-qt5/networkmanager.h>
#include <mlite5/MDesktopEntry>

#include <QDBusPendingCallWatcher>
#include <QtConcurrent>
#include <QProcess>

#include <QDebug>

using namespace PackageKit;

#define CHECK_INITIALISED(d) \
    Q_ASSERT_X((d)->initialised, Q_FUNC_INFO, "Call only after OrnPm was initialised!")

#define CHECK_NETWORK(RET) \
    if (!NetworkManager::instance()->connected()) { \
        qWarning("Network is unavailable!"); \
        return RET; \
    }

const QLatin1String OrnPm::repoNamePrefix("openrepos-");

OrnPm::OrnPm(QObject *parent)
    : QObject(*new OrnPmPrivate(), parent)
{
    Q_D(OrnPm);

    d->ssuInterface = new OrnSsu(this);

    d->pkDaemon = new OrnPkDaemon(this);
    connect_priv(d->pkDaemon, &OrnPkDaemon::UpdatesChanged, this, &OrnPmPrivate::getUpdates);
    connect(d->pkDaemon, &OrnPkDaemon::TransactionListChanged, this, [this](const QStringList &transactions) {
        static QSet<QString> active;
        for (const auto &path : transactions)
        {
            if (!active.contains(path))
            {
                active.insert(path);
                auto t = new OrnPkTransaction(path, true, this);
                connect_priv(t, &OrnPkTransaction::ItemProgress, this, &OrnPmPrivate::onItemProgress);
            }
        }
        active = transactions.toSet();
    });
}

QString OrnPm::repoUrl(const QString &author)
{
    return QStringLiteral("https://sailfish.openrepos.net/%0/personal/main").arg(author);
}

QString OrnPm::rpmQuery(const QString &packageFile, const QString &query)
{
    QProcess rpm;
    rpm.start(QStringLiteral("rpm"), {
        QStringLiteral("-qp"),
        QStringLiteral("--qf"),
        query,
        packageFile,
    });
    rpm.waitForFinished();
    return QString::fromUtf8(rpm.readAll());
}

OrnPm *OrnPm::instance()
{
    static OrnPm *instance = nullptr;
    if (!instance)
    {
        instance = new OrnPm(qApp);
        auto fw = new QFutureWatcher<void>();
        connect(fw, &QFutureWatcher<void>::finished, fw, [fw]()
        {
            fw->deleteLater();
            OrnPm::instance()->d_func()->getUpdates();
        });
        fw->setFuture(QtConcurrent::run(instance->d_func(), &OrnPmPrivate::initialise));
    }
    return instance;
}

void OrnPmPrivate::initialise()
{
    solvPathTmpl = OrnUtils::systemVersion() >= QVersionNumber(3, 0, 3) ?
        QStringLiteral("/home/.zypp-cache/solv/%0/solv") :
        QStringLiteral("/var/cache/zypp/solv/%0/solv");

    qDebug() << "Getting the list of ORN repositories";

    // NOTE: A hack for SSU repos. Can break on ssu config changes.
    QSettings ssuSettings(QStringLiteral("/etc/ssu/ssu.ini"), QSettings::IniFormat);

    archs << ssuSettings.value(QStringLiteral("arch")).toString()
          << QStringLiteral("noarch");

    auto disabled = ssuSettings.value(QStringLiteral("disabled-repos")).toStringList().toSet();
    ssuSettings.beginGroup(QStringLiteral("repository-urls"));
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
    auto srepo = repo_create(spool, OrnConst::installed.toLatin1().data());

    auto systemSolv = solvPathTmpl.arg(QStringLiteral("@System"));
    auto sfile = fopen(systemSolv.toUtf8().data(), "r");
    if (!sfile)
    {
        qCritical() << "Could not read" << systemSolv;
        repo_free(srepo, 0);
        pool_free(spool);
        return;
    }

    repo_add_solv(srepo, sfile, 0);
    fclose(sfile);

    for (int i = 0; i < spool->nsolvables; ++i)
    {
        auto s = &spool->solvables[i];
        auto name = QString::fromLatin1(solvable_lookup_str(s, SOLVABLE_NAME));
        if (name.size() > 0)
        {
            auto id = QStringLiteral("%1;%2;%3;installed").arg(
                        name,
                        QLatin1String{solvable_lookup_str(s, SOLVABLE_EVR)},
                        QLatin1String{solvable_lookup_str(s, SOLVABLE_ARCH)});
            installedPackages.insert(name, id);
        }
    }

    repo_free(srepo, 0);
    pool_free(spool);

    qDebug() << installedPackages.size() << "packages are installed";

    qDebug() << "Initialisation finished";
    initialised = true;
    emit this->q_func()->initialisedChanged();
}

bool OrnPm::initialised() const
{
    return this->d_func()->initialised;
}

QVariantList OrnPm::operations() const
{
    Q_D(const OrnPm);

    QString item     {QStringLiteral("item")};
    QString operation{QStringLiteral("operation")};
    QVariantList res;
    for (auto op = d->operations.cbegin(); op != d->operations.cend(); ++op)
    {
        res << QVariantMap{
            {item,      op.key()},
            {operation, op.value()}
        };
    }
    return res;
}

bool OrnPm::updatesAvailable() const
{
    return !this->d_func()->updatablePackages.empty();
}

QStringList OrnPm::updatablePackages() const
{
    return this->d_func()->updatablePackages.keys();
}

QString OrnPm::updateVersion(const QString &packageName) const
{
    Q_D(const OrnPm);

    if (d->updatablePackages.contains(packageName))
    {
        return OrnUtils::packageVersion(d->updatablePackages[packageName]);
    }
    return QString();
}

OrnPm::RepoStatus OrnPm::repoStatus(const QString &alias) const
{
    Q_D(const OrnPm);

    if (d->repos.contains(alias))
    {
        return d->repos[alias] ? RepoEnabled : RepoDisabled;
    }
    return RepoNotInstalled;
}

/*!
    This will never return \l OrnPm::PackageAvailable.
 */
OrnPm::PackageStatus OrnPm::packageStatus(const QString &packageName) const
{
    Q_D(const OrnPm);

    if (d->operations.contains(packageName))
    {
        switch (d->operations[packageName])
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

    if (d->updatablePackages.contains(packageName))
    {
        return PackageUpdateAvailable;
    }
    if (d->installedPackages.contains(packageName))
    {
        return PackageInstalled;
    }
    return PackageNotInstalled;
}

OrnPkTransaction *OrnPmPrivate::transaction()
{
    auto t = pkDaemon->transaction();
    QObject::connect(t, &OrnPkTransaction::ErrorCode, q_func(), &OrnPm::error);
    return t;
}

OrnPkTransaction *OrnPmPrivate::currentTransaction()
{
    return qobject_cast<OrnPkTransaction *>(currentSender->sender);
}

void OrnPm::getPackageVersions(const QString &packageName)
{
    Q_D(OrnPm);

    Q_ASSERT(!packageName.isEmpty());
    CHECK_INITIALISED(d);
    qDebug() << "Resolving package versions for" << packageName;

    QtConcurrent::run(d, &OrnPmPrivate::preparePackageVersions, packageName);
}

void OrnPmPrivate::preparePackageVersions(const QString &packageName)
{
    OrnPackageVersionList versions;
    auto spool = pool_create();
    auto srepo = repo_create(spool, "");

    auto systemSolv = solvPathTmpl.arg(QStringLiteral("@System"));
    auto sfile = fopen(systemSolv.toUtf8().data(), "r");
    if (!sfile)
    {
        qCritical() << "Could not read" << systemSolv;
        repo_free(srepo, 0);
        pool_free(spool);
        return;
    }
    repo_add_solv(srepo, sfile, 0);
    fclose(sfile);
    for (int i = 0; i < spool->nsolvables && versions.isEmpty(); ++i)
    {
        auto s = &spool->solvables[i];
        if (packageName == QLatin1String(solvable_lookup_str(s, SOLVABLE_NAME)))
        {
            versions << OrnPackageVersion(
                            0,
                            solvable_lookup_num(s, SOLVABLE_INSTALLSIZE, 0),
                            QString::fromLatin1(solvable_lookup_str(s, SOLVABLE_EVR)),
                            QString::fromLatin1(solvable_lookup_str(s, SOLVABLE_ARCH)),
                            OrnConst::installed);
        }
    }
    repo_free(srepo, 0);

    for (auto it = repos.cbegin(); it != repos.cend(); ++it)
    {
        if (it.value())
        {
            const auto &alias = it.key();
            auto spath = solvPathTmpl.arg(alias);
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
                if (packageName == QLatin1String(solvable_lookup_str(s, SOLVABLE_NAME)))
                {
                    auto arch = QString::fromLatin1(solvable_lookup_str(s, SOLVABLE_ARCH));
                    if (archs.contains(arch))
                    {
                        versions << OrnPackageVersion(
                                        solvable_lookup_num(s, SOLVABLE_DOWNLOADSIZE, 0),
                                        solvable_lookup_num(s, SOLVABLE_INSTALLSIZE, 0),
                                        QString::fromLatin1(solvable_lookup_str(s, SOLVABLE_EVR)),
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
    emit this->q_func()->packageVersions(packageName, versions);
}

void OrnPm::installPackage(const QString &packageId)
{
    CHECK_NETWORK();

    auto name = OrnUtils::packageName(packageId);

    auto t = d_func()->packageTransaction(name, OrnPm::InstallingPackage, OrnPm::PackageInstalling, &OrnPm::packageInstalled);
    if (t)
    {
        t->installPackages(QStringList{packageId});
    }
}

void OrnPm::installFile(const QString &packageFile)
{
    CHECK_NETWORK();

    // GetDetailsLocal is not supported for libzypp
    auto name = rpmQuery(packageFile, QStringLiteral("%{NAME}"));

    auto t = d_func()->packageTransaction(name, OrnPm::InstallingPackage, OrnPm::PackageInstalling, &OrnPm::packageInstalled);
    if (t)
    {
        t->installFiles(QStringList{packageFile});
    }
}

void OrnPm::removePackage(const QString &packageId, bool autoremove)
{
    auto name = OrnUtils::packageName(packageId);

    auto t = d_func()->packageTransaction(name, OrnPm::RemovingPackage, OrnPm::PackageRemoving, &OrnPm::packageRemoved);
    if (t)
    {
        t->removePackages(QStringList{packageId}, autoremove);
    }
}

void OrnPm::updatePackage(const QString &packageName)
{
    Q_D(OrnPm);

    CHECK_NETWORK();

    if (!d->updatablePackages.contains(packageName))
    {
        qWarning() << "The package" << packageName << "has no updates!";
        return;
    }

    auto t = d->packageTransaction(packageName, OrnPm::UpdatingPackage, OrnPm::PackageUpdating, &OrnPm::packageUpdated);
    if (t)
    {
        auto packageId = d->updatablePackages[packageName];
        t->updatePackages(QStringList{packageId});
    }
}

void OrnPm::addRepo(const QString &author)
{
    Q_D(OrnPm);

    CHECK_NETWORK();

    auto alias = repoNamePrefix + author;

    if (d->startOperation(alias, AddingRepo))
    {
        auto url = OrnPm::repoUrl(author);
        d->ssuInterface->addRepo(alias, url);
        d->onRepoModified(alias, AddRepo);
    }
}

OrnPm::Operation actionOperation(OrnPm::RepoAction action)
{
    switch (action)
    {
    case OrnPm::RemoveRepo:
        return OrnPm::RemovingRepo;
    case OrnPm::DisableRepo:
        return OrnPm::DisablingRepo;
    case OrnPm::EnableRepo:
        return OrnPm::EnablingRepo;
    default:
        Q_UNREACHABLE();
    }
}

void OrnPm::modifyRepo(const QString &alias, OrnPm::RepoAction action)
{
    Q_D(OrnPm);

    CHECK_NETWORK();

    if (d->startOperation(alias, actionOperation(action)))
    {
        d->ssuInterface->modifyRepo(action, alias);
        d->onRepoModified(alias, action);
    }
}

void OrnPm::enableRepos(bool enable)
{
    Q_D(OrnPm);

    CHECK_INITIALISED(d);

    auto watcher = new QFutureWatcher<bool>();
    auto future  = QtConcurrent::run(d, &OrnPmPrivate::enableRepos, enable);
    watcher->setFuture(future);
    connect(watcher, &QFutureWatcher<bool>::finished, this, [watcher, this]() {
        if (watcher->result())
        {
            this->refreshRepos();
        }
        watcher->deleteLater();
    });
}

void OrnPm::removeAllRepos()
{
    QtConcurrent::run(this->d_func(), &OrnPmPrivate::removeAllRepos);
}

bool OrnPm::refreshingCache() const
{
    for (auto op : d_func()->operations)
    {
        if (op == OrnPm::RefreshingRepo || op == OrnPm::RefreshingCache)
        {
            return true;
        }
    }
    return false;
}

bool OrnPmPrivate::enableRepos(bool enable)
{
    Q_Q(OrnPm);

    CHECK_NETWORK(false);
    qDebug() << (enable ? "Enabling" : "Disabling") << "all repositories";

    auto action  = enable ? OrnPm::EnableRepo : OrnPm::DisableRepo;
    bool refresh = false;

    for (auto it = repos.begin(); it != repos.end(); ++it)
    {
        if (*it != enable)
        {
            ssuInterface->modifyRepo(action, it.key());
            *it = enable;
            refresh = true;
        }
    }

    if (!enable && !updatablePackages.empty())
    {
        updatablePackages.clear();
        emit q->updatablePackagesChanged();
    }

    qDebug() << "Finished" << (enable ? "enabling" : "disabling") << "all repositories";
    emit q->enableReposFinished();

    return refresh;
}

void OrnPmPrivate::removeAllRepos()
{
    Q_Q(OrnPm);

    qDebug() << "Removing all repositories";

    for (auto it = repos.begin(); it != repos.end(); ++it)
    {
        ssuInterface->modifyRepo(OrnPm::RemoveRepo, it.key());
    }
    repos.clear();
    updatablePackages.clear();
    emit q->updatablePackagesChanged();

    qDebug() << "Finished removing all repositories";
    emit q->removeAllReposFinished();
}

void OrnPmPrivate::onRepoModified(const QString &alias, OrnPm::RepoAction action)
{
    Q_Q(OrnPm);

    bool needRefresh = false;

    switch (action)
    {
    case OrnPm::RemoveRepo:
        repos.remove(alias);
        break;
    case OrnPm::AddRepo:
        repos.insert(alias, true);
        needRefresh = true;
        break;
    case OrnPm::DisableRepo:
        repos[alias] = false;
        break;
    case OrnPm::EnableRepo:
        repos[alias] = true;
        needRefresh = true;
        break;
    }

    auto onFinish = [this, alias, action]() {
        qDebug() << "Repo" << alias << "have been modified with" << action;
        emit q_func()->repoModified(alias, action);
        finishOperation(alias);
    };

    if (needRefresh)
    {
        operations[alias] = OrnPm::RefreshingRepo;
        emit q->operationsChanged();
        auto t = this->transaction();
        QObject::connect(t, &OrnPkTransaction::Finished, onFinish);
        t->repoRefreshNow(alias);
    }
    else
    {
        onFinish();
        this->getUpdates();
    }
}

void OrnPm::refreshRepo(const QString &alias, bool force)
{
    Q_D(OrnPm);

    if (d->operations.contains(alias))
    {
        qDebug() << "Already processing repo";
        return;
    }

    CHECK_NETWORK();

    if (d->startOperation(alias, RefreshingRepo))
    {
        auto t = d->transaction();
        connect(t, &OrnPkTransaction::Finished, [this, alias]()
        {
            this->d_func()->finishOperation(alias);
        });
        t->repoRefreshNow(alias, force);
    }
}

void OrnPm::refreshRepos(bool force)
{
    Q_D(OrnPm);

    if (refreshingCache())
    {
        qDebug() << "Already refreshing cache";
        return;
    }

    CHECK_NETWORK();

    for (auto it = d->repos.cbegin(); it != d->repos.cend(); ++it)
    {
        // Refresh only enabled repositories
        if (it.value())
        {
            d->reposToRefresh << it.key();
        }
    }

    if (d->reposToRefresh.isEmpty())
    {
        qDebug() << "No enabled repositories, skipping";
        return;
    }

    qDebug() << "Starting refresh cache for all ORN repositories";

    d->pkDaemon->blockSignals(true);
    d->forceRefresh = OrnUtils::stringify(force);
#ifdef QT_DEBUG
    d->refreshRuntime = 0;
#endif
    d->refreshNextRepo(Transaction::ExitSuccess, 0);
}

void OrnPm::refreshCache(bool force)
{
    Q_D(OrnPm);

    if (refreshingCache())
    {
        qDebug() << "Already refreshing cache";
        return;
    }

    CHECK_NETWORK();

    QString name{QStringLiteral("__orn_refreshing_cache")};

    if (d->startOperation(name, OrnPm::RefreshingCache))
    {
        auto t = d->transaction();
        connect(t, &QObject::destroyed, [this, name]()
        {
            this->d_func()->finishOperation(name);
        });
        t->refreshCache(force);
    }
}

QList<OrnRepo> OrnPm::repoList() const
{
    Q_D(const OrnPm);

    OrnRepoList repos;
    auto pos = repoNamePrefix.size();
    for (auto it = d->repos.cbegin(); it != d->repos.cend(); ++it)
    {
        const auto &alias = it.key();
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
    watcher->setFuture(QtConcurrent::run(this->d_func(), &OrnPmPrivate::prepareInstalledPackages, packageName));
}

bool OrnPmPrivate::startOperation(const QString &name, OrnPm::Operation operation)
{
    CHECK_INITIALISED(this);
    if (operations.contains(name))
    {
        qWarning() << name << "is already being processed!";
        return false;
    }
    operations.insert(name, operation);
    emit this->q_func()->operationsChanged();
    return true;
}

void OrnPmPrivate::finishOperation(const QString &name)
{
    if (operations.remove(name))
    {
        emit q_func()->operationsChanged();
    }
}

OrnInstalledPackageList OrnPmPrivate::prepareInstalledPackages(const QString &packageName)
{
    Q_ASSERT_X(packageName.isEmpty() || installedPackages.contains(packageName), Q_FUNC_INFO,
               qPrintable(QString("The provided package \"%0\" is not installed").arg(packageName)));

    OrnInstalledPackageList packages;

    if (installedPackages.isEmpty() || repos.isEmpty())
    {
        qWarning() << "Installed packages or repositories list is empty";
        return packages;
    }
    qDebug() << "Preparing installed packages list";

    QStringList iconPaths{
        QStringLiteral("/usr/share/icons/hicolor/86x86/apps/%0.png"),
        QStringLiteral("/usr/share/icons/hicolor/108x108/apps/%0.png"),
        QStringLiteral("/usr/share/icons/hicolor/128x128/apps/%0.png"),
        QStringLiteral("/usr/share/icons/hicolor/172x172/apps/%0.png"),
        QStringLiteral("/usr/share/icons/hicolor/256x256/apps/%0.png"),
    };

    // Prepare set to filter installed packages to show only those from OpenRepos
    StringSet ornPackages;
    auto spool = pool_create();
    for (auto it = repos.cbegin(); it != repos.cend(); ++it)
    {
        if (it.value())
        {
            const auto &alias = it.key();
            auto spath = solvPathTmpl.arg(alias);
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
                ornPackages.insert(QString::fromLatin1(solvable_lookup_str(s, SOLVABLE_NAME)));
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

        QString title{name};
        QString icon;
        MDesktopEntry desktop{OrnUtils::desktopFile(name)};

        if (desktop.isValid())
        {
            // Read pretty name
            title = desktop.name();
            qDebug() << "Using name" << title << "for package" << name;
            // Find icon
            auto iconName = desktop.icon();
            if (!iconName.isEmpty())
            {
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
        packages << OrnInstalledPackage {
            updatablePackages.contains(name),
            it.value(),
            name,
            title,
            icon
        };
    }

    return packages;
}

void OrnPmPrivate::onItemProgress(const QString &id, uint status, uint percentage)
{
    if (   percentage < 100
        || !(status == Transaction::StatusRemove || status == Transaction::StatusInstall || status == Transaction::StatusUpdate))
    {
        return;
    }

    Q_Q(OrnPm);

    auto name = OrnUtils::packageName(id);

    if (status == Transaction::StatusRemove)
    {
        installedPackages.remove(name);
        emit q->packageStatusChanged(name, OrnPm::PackageNotInstalled);
        return;
    }

    installedPackages[name] = id;
    emit q->packageStatusChanged(name, OrnPm::PackageInstalled);
    if (updatablePackages.contains(name))
    {
        getUpdates();
    }
}

OrnPkTransaction *OrnPmPrivate::packageTransaction(const QString &packageName,
        OrnPm::Operation operation,
        OrnPm::PackageStatus status,
        OrnPmPrivate::ornpm_signal_t sgnl)
{
    if (!startOperation(packageName, operation))
    {
        return nullptr;
    }

    Q_Q(OrnPm);

    emit q->packageStatusChanged(packageName, status);
    auto t = transaction();
    QObject::connect(t, &OrnPkTransaction::Finished, q, [this, packageName, sgnl](uint status) {
        Q_Q(OrnPm);
        operations.remove(packageName);
        emit q->operationsChanged();
        if (status == Transaction::ExitSuccess) {
            emit (q->*sgnl)(packageName);
        }
    });
    return t;
}

void OrnPmPrivate::getUpdates()
{
    CHECK_NETWORK();

    Q_Q(OrnPm);

    auto t = this->transaction();

    QObject::connect(t, &OrnPkTransaction::Package, q, [this](quint32 info, const QString &packageId, [[maybe_unused]] const QString &summary) {
        Q_ASSERT(info == Transaction::InfoEnhancement);
        // Filter updates only for ORN packages
        if (OrnUtils::packageRepo(packageId).startsWith(OrnPm::repoNamePrefix))
        {
            newUpdatablePackages.insert(OrnUtils::packageName(packageId), packageId);
        }
    });

    QObject::connect(t, &OrnPkTransaction::Finished, q, [this](quint32 status, [[maybe_unused]] quint32 runtime) {
        if (status != Transaction::ExitSuccess)
        {
            newUpdatablePackages.clear();
            return;
        }

        Q_Q(OrnPm);

        bool newupdates = false;
        // If some client listen to packageStatusChanged() and want to take a package
        // update ID we swap two hashes before the checks
        updatablePackages.swap(newUpdatablePackages);
        auto it = updatablePackages.begin();
        while (it != updatablePackages.end())
        {
            auto &name = it.key();
            auto &id   = it.value();
            auto repo  = OrnUtils::packageRepo(id);
            // A walkaround to skip inactual updates from removed/disabled repos
            if (!repos.contains(repo) || !repos[repo])
            {
                it = updatablePackages.erase(it);
            }
            else
            {
                // Check if the update is really new
                if (!newUpdatablePackages.contains(name) ||
                    newUpdatablePackages[name] != id)
                {
                    emit q->packageStatusChanged(name, OrnPm::PackageUpdateAvailable);
                    newupdates = true;
                }
                ++it;
            }
        }
        if (newupdates ||
            (updatablePackages.size() != newUpdatablePackages.size()))
        {
            emit q->updatablePackagesChanged();
        }
        newUpdatablePackages.clear();
    });

    t->getUpdates();
}

void OrnPmPrivate::refreshNextRepo(quint32 exit, quint32 runtime)
{
    Q_Q(OrnPm);

    Q_UNUSED(exit)

#ifdef QT_DEBUG
    refreshRuntime += runtime;
#else
    Q_UNUSED(runtime)
#endif

    bool offline = !NetworkManager::instance()->connected();
    if (offline)
    {
        reposToRefresh.clear();
    }
    if (reposToRefresh.isEmpty())
    {
#ifdef QT_DEBUG
        if (offline)
        {
            qWarning("Aborting operation due to network gone offline");
        }
        else
        {
            qDebug() << "Finished refreshing cache for all ORN repositories in"
                     << refreshRuntime << "msec";
        }
#endif
        pkDaemon->blockSignals(false);
    }
    else
    {
        auto alias = reposToRefresh.takeFirst();
        auto t = this->transaction();
        QObject::connect(t, &OrnPkTransaction::Finished, q, [this, alias](quint32 exit, quint32 runtime) {
            refreshNextRepo(exit, runtime);
            finishOperation(alias);
        });
        startOperation(alias, OrnPm::RefreshingRepo);
        t->repoRefreshNow(alias, forceRefresh);
    }
}
