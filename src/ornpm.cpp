#include "ornpm_p.h"
#include "ornpackageversion.h"
#include "ornrepo.h"
#include "ornutils.h"
#include "ornconst.h"

#include <solv/repo_solv.h>
#include <connman-qt5/networkmanager.h>

#include <QtConcurrent/QtConcurrent>

#include <QDebug>

using namespace PackageKit;

#define CHECK_INITIALISED() \
    Q_ASSERT_X(d->initialised, Q_FUNC_INFO, "Call only after OrnPm was initialised!")

#define CHECK_NETWORK(RET) \
    if (NetworkManager::instance()->state() != QLatin1String("online")) { \
        qWarning("Network is unavailable!"); \
        return RET; \
    }

#define SET_OPERATION_ITEM(operation, item) \
    CHECK_INITIALISED(); \
    if (d->operations.contains(item)) { \
        qWarning() << (item) << "is already being processed!"; \
        return; \
    } \
    d->operations.insert(item, operation); \
    emit this->operationsChanged();

const QLatin1String OrnPm::repoNamePrefix("openrepos-");

OrnPm::OrnPm(QObject *parent)
    : QObject(*new OrnPmPrivate(), parent)
{
    Q_D(OrnPm);

    auto bus = QDBusConnection::systemBus();

    d->ssuInterface = new SsuInterface(this);

    d->pkInterface = new QDBusInterface(OrnConst::pkService, QStringLiteral("/org/freedesktop/PackageKit"), OrnConst::pkService, bus, this);
    QObject::connect(d->pkInterface, SIGNAL(UpdatesChanged()), this, SLOT(getUpdates()));
}

QString OrnPm::repoUrl(const QString &author)
{
    return QStringLiteral("https://sailfish.openrepos.net/%0/personal/main").arg(author);
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

QDBusInterface *OrnPmPrivate::transaction(const QString &item)
{
    Q_Q(OrnPm);

    auto reply = pkInterface->call(QStringLiteral("CreateTransaction"));
    Q_ASSERT_X(reply.type() != QDBusMessage::ErrorMessage, Q_FUNC_INFO,
               qPrintable(reply.errorName().append(": ").append(reply.errorMessage())));

    auto t = new QDBusInterface(OrnConst::pkService,
                                qvariant_cast<QDBusObjectPath>(reply.arguments().first()).path(),
                                QStringLiteral("org.freedesktop.PackageKit.Transaction"),
                                QDBusConnection::systemBus(),
                                q);
    Q_ASSERT(t->isValid());
#ifdef QT_DEBUG
    QObject::connect(t, SIGNAL(Finished(quint32,quint32)),  q, SLOT(onTransactionFinished(quint32,quint32)));
    QObject::connect(t, SIGNAL(ErrorCode(quint32,QString)), q, SLOT(emitError(quint32,QString)));
#else
    QObject::connect(t, SIGNAL(Finished(quint32, quint32)), t, SLOT(deleteLater()));
    QObject::connect(t, SIGNAL(ErrorCode(quint32,QString)), q, SIGNAL(error(quint32,QString)));
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

void OrnPm::getPackageVersions(const QString &packageName)
{
    Q_D(OrnPm);

    Q_ASSERT(!packageName.isEmpty());
    CHECK_INITIALISED();
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
    Q_D(OrnPm);

    CHECK_NETWORK();
    SET_OPERATION_ITEM(InstallingPackage, OrnUtils::packageName(packageId));

    auto t = d->transaction(packageId);
    connect(t, SIGNAL(Finished(quint32,quint32)), this, SLOT(onPackageInstalled(quint32,quint32)));
    QStringList ids(packageId);
    qDebug().nospace().noquote()
            << "Calling " << t << "->" << OrnConst::pkInstallPackages << "(" << PK_FLAG_NONE << ", " << ids << ")";
    emit this->packageStatusChanged(OrnUtils::packageName(packageId), OrnPm::PackageInstalling);
    t->asyncCall(OrnConst::pkInstallPackages, PK_FLAG_NONE, ids);
}

void OrnPm::installFile(const QString &packageFile)
{
    Q_D(OrnPm);

    CHECK_NETWORK();

    auto t = d->transaction();
    connect(t, SIGNAL(Finished(quint32,quint32)), this, SLOT(onPackageInstalled(quint32,quint32)));
    QString method{QStringLiteral("InstallFiles")};
    QStringList files(packageFile);    
    qDebug().nospace().noquote() << "Calling " << t << "->" << method << "(" << PK_FLAG_NONE << ", " << files << ")";
    t->asyncCall(method, PK_FLAG_NONE, files);
}

void OrnPm::removePackage(const QString &packageId, bool autoremove)
{
    Q_D(OrnPm);

    SET_OPERATION_ITEM(RemovingPackage, OrnUtils::packageName(packageId));

    auto t = d->transaction(packageId);
    connect(t, SIGNAL(Finished(quint32,quint32)), this, SLOT(onPackageRemoved(quint32,quint32)));
    QString method{QStringLiteral("RemovePackages")};
    QStringList ids(packageId);
    qDebug().nospace().noquote()
            << "Calling " << t << "->" << method << "("
            << PK_FLAG_NONE << ", " << ids << ", false, " << autoremove << ")";
    emit this->packageStatusChanged(OrnUtils::packageName(packageId), OrnPm::PackageRemoving);
    t->asyncCall(method, PK_FLAG_NONE, ids, false, autoremove);
}

void OrnPm::updatePackage(const QString &packageName)
{
    Q_D(OrnPm);

    if (!d->updatablePackages.contains(packageName))
    {
        qWarning() << "The package" << packageName << "has no updates!";
        return;
    }
    CHECK_NETWORK();
    SET_OPERATION_ITEM(UpdatingPackage, packageName);

    auto packageId = d->updatablePackages[packageName];
    auto t = d->transaction(packageId);
    connect(t, SIGNAL(Finished(quint32,quint32)), this, SLOT(onPackageUpdated(quint32,quint32)));
    QStringList ids(packageId);
    qDebug().nospace().noquote()
            << "Calling " << t << "->" << OrnConst::pkInstallPackages << "(" << PK_FLAG_NONE << ", " << ids << ")";
    emit this->packageStatusChanged(packageName, OrnPm::PackageUpdating);
    t->asyncCall(OrnConst::pkInstallPackages, PK_FLAG_NONE, ids);
}

void OrnPm::addRepo(const QString &author)
{
    Q_D(OrnPm);

    CHECK_NETWORK();
    CHECK_INITIALISED();

    auto alias   = repoNamePrefix + author;
    SET_OPERATION_ITEM(AddingRepo, alias);
    auto url     = OrnPm::repoUrl(author);
    auto watcher = d->ssuInterface->addRepoAsync(alias, url);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher, alias]()
    {
        this->d_func()->onRepoModified(alias, AddRepo);
        watcher->deleteLater();
    });
}

void OrnPm::modifyRepo(const QString &alias, OrnPm::RepoAction action)
{
    Q_D(OrnPm);

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
    SET_OPERATION_ITEM(op, alias);

    auto watcher = d->ssuInterface->modifyRepoAsync(action, alias);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher, alias, action]()
    {
        this->d_func()->onRepoModified(alias, action);
        watcher->deleteLater();
    });
}

void OrnPm::enableRepos(bool enable)
{
    Q_D(OrnPm);

    CHECK_INITIALISED();

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

    qDebug() <<"Removing all repositories";

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

void OrnPmPrivate::onRepoModified(const QString &repoAlias, OrnPm::RepoAction action)
{
    Q_Q(OrnPm);

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
        emit q->operationsChanged();
        auto t = this->transaction();
        qDebug().nospace().noquote()
                << "Calling " << t << "->" << OrnConst::pkRepoSetData
                << "(\"" << repoAlias << "\", \"refresh-now\", false)";
        t->asyncCall(OrnConst::pkRepoSetData, repoAlias, "refresh-now", "false");
        QObject::connect(t, &QDBusInterface::destroyed, [this, repoAlias, action]()
        {
            Q_Q(OrnPm);
            operations.remove(repoAlias);
            emit q->operationsChanged();
            emit q->repoModified(repoAlias, action);
            qDebug() << "Repo" << repoAlias << "have been modified with" << action;
        });
    }
    else
    {
        operations.remove(repoAlias);
        emit q->operationsChanged();
        emit q->repoModified(repoAlias, action);
        qDebug() << "Repo" << repoAlias << "have been modified with" << action;
        this->getUpdates();
    }
}

void OrnPm::refreshRepo(const QString &alias, bool force)
{
    Q_D(OrnPm);

    CHECK_NETWORK();
    SET_OPERATION_ITEM(RefreshingRepo, alias);
    auto t = d->transaction();
    connect(t, &QDBusInterface::destroyed, [this, alias]()
    {
        this->d_func()->operations.remove(alias);
        emit this->operationsChanged();
    });
    qDebug().nospace() << "Calling " << t << "->RepoSetData(" << alias
                       << ", \"refresh-now\", " << (force ? "true" : "false") << ")";
    t->asyncCall("RepoSetData", alias, "refresh-now", OrnUtils::stringify(force));
}

void OrnPm::refreshRepos(bool force)
{
    Q_D(OrnPm);

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

    d->pkInterface->blockSignals(true);
    d->forceRefresh = OrnUtils::stringify(force);
#ifdef QT_DEBUG
    d->refreshRuntime = 0;
#endif
    d->refreshNextRepo(Transaction::ExitSuccess, 0);
}

void OrnPm::refreshCache(bool force)
{
    Q_D(OrnPm);

    CHECK_NETWORK();

    QString name{QStringLiteral("__orn_refreshing_cache")};

    SET_OPERATION_ITEM(OrnPm::RefreshingCache, name);

    auto t = d->transaction();
    connect(t, &QDBusInterface::destroyed, [this, name]()
    {
        this->d_func()->operations.remove(name);
        emit this->operationsChanged();
    });
    qDebug().nospace().noquote()
            << "Calling " << t << "->" << OrnConst::pkRefreshCache << "(" << OrnUtils::stringify(force) << ")";
    t->asyncCall(OrnConst::pkRefreshCache, force);
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

OrnInstalledPackageList OrnPmPrivate::prepareInstalledPackages(const QString &packageName)
{
    Q_ASSERT_X(packageName.isEmpty() || installedPackages.contains(packageName), Q_FUNC_INFO,
               qPrintable(QString("The provided package \"%0\" is not installed").arg(packageName)));

    OrnInstalledPackageList packages;

    if (installedPackages.isEmpty() || repos.isEmpty())
    {
        qWarning() << "Installed packages or repositories list is empty";
        emit this->q_func()->installedPackages(packages);
        return packages;
    }
    qDebug() << "Preparing installed packages list";

    // Prepare vars for parsing desktop files
    QString nameKey{QStringLiteral("Desktop Entry/Name")};
    auto trNameKey = QString(nameKey).append("[%0]");
    auto localeName = QLocale::system().name();
    auto localeNameKey = trNameKey.arg(localeName);
    QString langNameKey;
    if (localeName.length() > 2)
    {
        langNameKey = trNameKey.arg(localeName.left(2));
    }
    QString iconKey{QStringLiteral("Desktop Entry/Icon")};
    QStringList iconPaths = {
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
        const auto &id = it.value();
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

void OrnPmPrivate::getUpdates()
{
    Q_Q(OrnPm);

    CHECK_NETWORK();
    auto t = this->transaction();
    QObject::connect(t, SIGNAL(Package(quint32,QString,QString)), q, SLOT(onPackageUpdate(quint32,QString,QString)));
    QObject::connect(t, SIGNAL(Finished(quint32,quint32)), q, SLOT(onGetUpdatesFinished(quint32,quint32)));
    QString method{QStringLiteral("GetUpdates")};
    qDebug().nospace().noquote() << "Calling " << t << "->" << method << "(" << PK_FLAG_NONE << ")";
    t->asyncCall(method, PK_FLAG_NONE);
}

void OrnPmPrivate::onPackageUpdate(quint32 info, const QString &packageId, const QString &summary)
{
    Q_UNUSED(summary)
    Q_ASSERT(info == Transaction::InfoEnhancement);
    // Filter updates only for ORN packages
    if (OrnUtils::packageRepo(packageId).startsWith(OrnPm::repoNamePrefix))
    {
        this->newUpdatablePackages.insert(OrnUtils::packageName(packageId), packageId);
    }
}

void OrnPmPrivate::onGetUpdatesFinished(quint32 status, quint32 runtime)
{
    Q_UNUSED(runtime)
    if (status == Transaction::ExitSuccess)
    {
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
    }
    newUpdatablePackages.clear();
}

void OrnPmPrivate::onPackageInstalled(quint32 exit, quint32 runtime)
{
    Q_Q(OrnPm);

    Q_UNUSED(runtime)
    auto id = transactionHash.take(q->sender());
    auto name = OrnUtils::packageName(id);
    operations.remove(name);
    emit q->operationsChanged();
    if (exit == Transaction::ExitSuccess)
    {
        installedPackages[name] = id;
        emit q->packageInstalled(name);
        emit q->packageStatusChanged(name, OrnPm::PackageInstalled);
    }
    else
    {
        emit q->packageStatusChanged(name, OrnPm::PackageUnknownStatus);
    }
}

void OrnPmPrivate::onPackageRemoved(quint32 exit, quint32 runtime)
{
    Q_Q(OrnPm);

    Q_UNUSED(runtime)
    auto id = transactionHash.take(q->sender());
    auto name = OrnUtils::packageName(id);
    operations.remove(name);
    emit q->operationsChanged();
    if (exit == Transaction::ExitSuccess)
    {
        installedPackages.remove(name);
        emit q->packageRemoved(name);
        emit q->packageStatusChanged(name, OrnPm::PackageNotInstalled);
    }
    else
    {
        emit q->packageStatusChanged(name, OrnPm::PackageUnknownStatus);
    }
}

void OrnPmPrivate::onPackageUpdated(quint32 exit, quint32 runtime)
{
    Q_Q(OrnPm);

    Q_UNUSED(runtime)
    auto id = transactionHash.take(q->sender());
    auto name = OrnUtils::packageName(id);
    operations.remove(name);
    emit q->operationsChanged();
    if (exit == Transaction::ExitSuccess)
    {
        updatablePackages.remove(name);
        installedPackages[name] = id;
        emit q->packageUpdated(name);
        emit q->packageStatusChanged(name, OrnPm::PackageInstalled);
        emit q->updatablePackagesChanged();
    }
    else
    {
        emit q->packageStatusChanged(name, OrnPm::PackageUnknownStatus);
    }
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

    bool offline = NetworkManager::instance()->state() != QLatin1String("online");
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
        pkInterface->blockSignals(false);
    }
    else
    {
        auto t = this->transaction();
        QObject::connect(t, SIGNAL(Finished(quint32,quint32)), q, SLOT(refreshNextRepo(quint32,quint32)));
        auto alias = reposToRefresh.takeFirst();
        QObject::connect(t, &QDBusInterface::destroyed, q, [this, alias]()
        {
            operations.remove(alias);
            emit this->q_func()->operationsChanged();
        });
        operations.insert(alias, OrnPm::RefreshingRepo);
        emit q->operationsChanged();
        qDebug().nospace().noquote()
                << "Calling " << t << "->" << OrnConst::pkRepoSetData
                << "(\"" << alias << "\", \"refresh-now\", " << forceRefresh << ")";
        t->asyncCall(OrnConst::pkRepoSetData, alias, "refresh-now", forceRefresh);
    }
}

#include <moc_ornpm.cpp>
