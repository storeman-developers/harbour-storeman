#include "ornbackup.h"
#include "ornpm.h"
#include "ornpm_p.h"
#include "ornpackageversion.h"
#include "ornclient.h"
#include "ornclient_p.h"
#include "ornutils.h"

#include <QFileInfo>
#include <QSettings>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>

#include <QDebug>

#define BR_CREATED       QStringLiteral("created")
#define BR_REPO_ALL      QStringLiteral("repos/all")
#define BR_REPO_DISABLED QStringLiteral("repos/disabled")
#define BR_INSTALLED     QStringLiteral("packages/installed")
#define BR_BOOKMARKS     QStringLiteral("packages/bookmarks")

OrnBackup::OrnBackup(QObject *parent)
    : QObject(parent)
    , mStatus(Idle)
{}

OrnBackup::Status OrnBackup::status() const
{
    return mStatus;
}

void OrnBackup::setStatus(Status status)
{
    if (mStatus != status)
    {
        mStatus = status;
        emit this->statusChanged();
    }
}

QVariantMap OrnBackup::details(const QString &path)
{
    Q_ASSERT_X(QFileInfo(path).isFile(), Q_FUNC_INFO, "Backup file does not exist");

    QVariantMap res;
    QSettings file(path, QSettings::IniFormat);

    res.insert(QLatin1String("created"),   file.value(BR_CREATED).toDateTime().toLocalTime());
    res.insert(QLatin1String("repos"),     file.value(BR_REPO_ALL).toStringList().size());
    res.insert(QLatin1String("packages"),  file.value(BR_INSTALLED).toStringList().size());
    res.insert(QLatin1String("bookmarks"), file.value(BR_BOOKMARKS).toStringList().size());

    return res;
}

void OrnBackup::backup(const QString &filePath, BackupItems items)
{
    Q_ASSERT_X(!filePath.isEmpty(), Q_FUNC_INFO, "A file path must be provided");
    Q_ASSERT_X(!QFileInfo(filePath).isFile(), Q_FUNC_INFO, "Backup file already exists");
    Q_ASSERT_X(int(items) > 0, Q_FUNC_INFO, "At least one backup item should be provided");

    if (mStatus != Idle)
    {
        qWarning() << this << "is already" << mStatus;
        return;
    }

    auto dir = QFileInfo(filePath).dir();
    if (!dir.exists() && !dir.mkpath(QChar('.')))
    {
        qCritical() << "Failed to create directory" << dir.absolutePath();
        emit this->backupError(DirectoryError);
    }
    QtConcurrent::run(this, &OrnBackup::pBackup, filePath, items);
}

void OrnBackup::restore(const QString &filePath)
{
    Q_ASSERT_X(!filePath.isEmpty(), Q_FUNC_INFO, "A file path must be set");
    Q_ASSERT_X(QFileInfo(filePath).isFile(), Q_FUNC_INFO, "Backup file does not exist");

    if (mStatus != Idle)
    {
        qWarning() << this << "is already" << mStatus;
        return;
    }

    auto watcher = new QFutureWatcher<void>();
    connect(watcher, &QFutureWatcher<void>::finished, this, &OrnBackup::pRefreshRepos);
    watcher->setFuture(QtConcurrent::run(this, &OrnBackup::pRestore, filePath));
}

QStringList OrnBackup::notFound() const
{
    QStringList names;
    for (const auto &name : mPackagesToInstall.keys())
    {
        if (!mNamesToSearch.contains(name))
        {
            names << name;
        }
    }
    return names;
}

void OrnBackup::pSearchPackages()
{
    qDebug() << "Searching packages";
    this->setStatus(SearchingPackages);

    // Delete future watcher and prepare variables
    this->sender()->deleteLater();
    mPackagesToInstall.clear();

    auto t = OrnPm::instance()->d_func()->transaction();
    connect(t, SIGNAL(Package(quint32,QString,QString)), this, SLOT(pAddPackage(quint32,QString,QString)));
    connect(t, SIGNAL(Finished(quint32,quint32)), this, SLOT(pInstallPackages()));
    qDebug().nospace() << "Calling " << t << "->" PK_METHOD_RESOLVE "("
                       << PK_FLAG_NONE << ", " << mNamesToSearch << ")";
    t->asyncCall(QStringLiteral(PK_METHOD_RESOLVE), PK_FLAG_NONE, mNamesToSearch);
}

void OrnBackup::pAddPackage(quint32 info, const QString &packageId, const QString &summary)
{
    Q_UNUSED(info)
    Q_UNUSED(summary)
    auto name = OrnUtils::packageName(packageId);
    if (mNamesToSearch.contains(name))
    {
        auto repo = OrnUtils::packageRepo(packageId);
        // Process only packages from OpenRepos
        if (repo.startsWith(OrnPm::repoNamePrefix))
        {
            // We will filter the newest versions later
            mPackagesToInstall.insert(name, packageId);
        }
        else if (repo == QStringLiteral("installed"))
        {
            mInstalled.insert(name, OrnUtils::packageVersion(packageId));
        }
    }
}

void OrnBackup::pInstallPackages()
{
    QStringList ids;
    for (const auto &pname : mPackagesToInstall.uniqueKeys())
    {
        const auto &pids = mPackagesToInstall.values(pname);
        QString newestId;
        OrnPackageVersion newestVersion;
        for (const auto &pid : pids)
        {
            OrnPackageVersion v(OrnUtils::packageVersion(pid));
            if (newestVersion < v)
            {
                newestVersion = v;
                newestId = pid;
            }
        }
        // Skip packages that are already installed
        if (!mInstalled.contains(pname) ||
                OrnPackageVersion(mInstalled[pname]) < newestVersion)
        {
            ids << newestId;
        }
    }

    if (ids.isEmpty())
    {
        this->pFinishRestore();
    }
    else
    {
        qDebug() << "Installing packages";
        this->setStatus(InstallingPackages);
        auto t = OrnPm::instance()->d_func()->transaction();
        connect(t, SIGNAL(Finished(quint32,quint32)), this, SLOT(pFinishRestore()));
        qDebug().nospace() << "Calling " << t << "->" PK_METHOD_INSTALLPACKAGES "("
                           << PK_FLAG_NONE << ", " << ids << ")";
        t->call(QStringLiteral(PK_METHOD_INSTALLPACKAGES), PK_FLAG_NONE, ids);
    }
}

void OrnBackup::pFinishRestore()
{
    qDebug() << "Finished restoring";
    this->setStatus(Idle);
    emit this->restored();
}

void OrnBackup::pBackup(const QString &filePath, BackupItems items)
{
    qDebug() << "Starting backing up";
    this->setStatus(BackingUp);
    QSettings file(filePath, QSettings::IniFormat);
    auto ornpm_p = OrnPm::instance()->d_func();

    if (items.testFlag(BackupItem::BackupRepos))
    {
        qDebug() << "Backing up repos";
        QStringList repos;
        QStringList disabled;
        auto prefix_size = OrnPm::repoNamePrefix.size();
        for (auto it = ornpm_p->repos.cbegin(); it != ornpm_p->repos.cend(); ++it)
        {
            auto author = it.key().mid(prefix_size);
            repos << author;
            if (!it.value())
            {
                disabled << author;
            }
        }
        file.setValue(BR_REPO_ALL, repos);
        file.setValue(BR_REPO_DISABLED, disabled);
    }

    if (items.testFlag(BackupItem::BackupInstalled))
    {
        qDebug() << "Backing up installed packages";
        QStringList installed;
        for (const auto &p : ornpm_p->prepareInstalledPackages(QString()))
        {
            installed << p.name;
        }
        file.setValue(BR_INSTALLED, installed);
    }

    if (items.testFlag(BackupItem::BackupBookmarks))
    {
        qDebug() << "Backing up bookmarks";
        QVariantList bookmarks;
        for (const auto &b : OrnClient::instance()->d_func()->bookmarks)
        {
            bookmarks << b;
        }
        file.setValue(BR_BOOKMARKS, bookmarks);
    }

    file.setValue(BR_CREATED, QDateTime::currentDateTime().toUTC());

    qDebug() << "Finished backing up";
    this->setStatus(Idle);
    emit this->backedUp();
}

void OrnBackup::pRestore(const QString &filePath)
{
    QSettings file(filePath, QSettings::IniFormat);

    qDebug() << "Reading bookmarks";
    auto bookmarks = file.value(BR_BOOKMARKS).toList();
    if (!bookmarks.isEmpty())
    {
        qDebug() << "Restoring bookmarks";
        this->setStatus(RestoringBookmarks);
        auto client = OrnClient::instance();
        for (const auto &b : bookmarks)
        {
            client->d_func()->bookmarks.insert(b.toUInt());
        }
    }

    qDebug() << "Reading repos";
    auto repos = file.value(BR_REPO_ALL).toStringList();
    if (!repos.isEmpty())
    {
        qDebug() << "Restoring repos";
        this->setStatus(RestoringRepos);
        auto disabled = file.value(BR_REPO_DISABLED).toStringList().toSet();
        auto ornpm_p = OrnPm::instance()->d_func();
        QString method(SSU_METHOD_ADDREPO);
        QString repo_tmpl(REPO_URL_TMPL);
        for (const auto &author : repos)
        {
            auto alias = OrnPm::repoNamePrefix + author;
            ornpm_p->ssuInterface->call(QDBus::Block, method, alias, repo_tmpl.arg(author));
            ornpm_p->repos.insert(alias, !disabled.contains(author));
        }

    }

    qDebug("Reading installed apps");
    mNamesToSearch = file.value(BR_INSTALLED).toStringList();
}

void OrnBackup::pRefreshRepos()
{
    if (!mNamesToSearch.isEmpty())
    {
        qDebug() << "Refreshing repos";
        this->setStatus(RefreshingRepos);
        auto t = OrnPm::instance()->d_func()->transaction();
        connect(t, SIGNAL(Finished(quint32,quint32)), this, SLOT(pSearchPackages()));
        qDebug().nospace() << "Calling " << t << "->" PK_METHOD_REFRESHCACHE "(false)";
        t->asyncCall(QStringLiteral(PK_METHOD_REFRESHCACHE), false);
    }
    else
    {
        this->pFinishRestore();
    }
}
