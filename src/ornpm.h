#ifndef ORNPM_H
#define ORNPM_H

#include <QObject>

#include <Transaction>

class QQmlEngine;
class QJSEngine;
class OrnPackageVersion;
class OrnInstalledPackage;
class OrnRepo;

struct OrnPmPrivate;

class OrnPm : public QObject
{
    friend struct OrnPmPrivate;
    friend class OrnBackup;

    Q_OBJECT

    Q_PROPERTY(bool initialised READ initialised NOTIFY initialisedChanged)
    Q_PROPERTY(QVariantList operations READ operations NOTIFY operationsChanged)
    Q_PROPERTY(QString deviceModel READ deviceModel CONSTANT)
    Q_PROPERTY(bool updatesAvailable READ updatesAvailable NOTIFY updatablePackagesChanged)

public:

    enum Operation
    {
        NoOperations,
        AddingRepo,
        RemovingRepo,
        EnablingRepo,
        DisablingRepo,
        RefreshingRepo,
        InstallingPackage,
        RemovingPackage,
        UpdatingPackage
    };
    Q_ENUM(Operation)

    // ssu actions
    enum RepoAction
    {
        RemoveRepo  = 0,
        AddRepo     = 1,
        DisableRepo = 2,
        EnableRepo  = 3
    };
    Q_ENUM(RepoAction)

    enum RepoStatus
    {
        RepoUnknownStatus,
        RepoNotInstalled,
        RepoDisabled,
        RepoEnabled
    };
    Q_ENUM(RepoStatus)

    enum PackageStatus
    {
        PackageUnknownStatus,
        PackageNotInstalled,
        PackageAvailable,
        PackageInstalled,
        PackageUpdateAvailable,
        PackageInstalling,
        PackageRemoving,
        PackageUpdating
    };
    Q_ENUM(PackageStatus)

    enum ErrorCode
    {
        ErrorUnknown = PackageKit::Transaction::ErrorUnknown,
        ErrorPackageNotFound = PackageKit::Transaction::ErrorPackageNotFound,
        ErrorDepResolutionFailed = PackageKit::Transaction::ErrorDepResolutionFailed
    };
    Q_ENUM(ErrorCode)

    static const QLatin1String repoNamePrefix;

    static OrnPm *instance();
    static inline QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
    {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return OrnPm::instance();
    }

    bool initialised() const;
    QVariantList operations() const;

    QString deviceModel() const;

    bool updatesAvailable() const;
    Q_INVOKABLE QStringList updatablePackages() const;
    QString updateVersion(const QString &packageName) const;

    RepoStatus repoStatus(const QString &alias) const;
    PackageStatus packageStatus(const QString &packageName) const;

signals:
    void initialisedChanged();
    void operationsChanged();
    void packageStatusChanged(const QString &packageName, const PackageStatus &status);
    void error(quint32 code, const QString &details);

private slots:
#ifdef QT_DEBUG
    void onTransactionFinished(quint32 exit, quint32 runtime);
    void emitError(quint32 code, const QString& details);
#endif

    // Check for updates
signals:
    void updatablePackagesChanged();
private slots:
    void getUpdates();
    void onPackageUpdate(quint32 info, const QString& packageId, const QString &summary);
    void onGetUpdatesFinished(quint32 status, quint32 runtime);

    // Package versions
signals:
    void packageVersions(const QString &packageName, const QList<OrnPackageVersion> &versions);
public slots:
    void getPackageVersions(const QString &packageName);

    // Install package
signals:
    void packageInstalled(const QString &packageName);
public slots:
    void installPackage(const QString &packageId);
    void installFile(const QString &packageFile);
private slots:
    void onPackageInstalled(quint32 exit, quint32 runtime);

    // Remove package
signals:
    void packageRemoved(const QString &packageName);
public slots:
    void removePackage(const QString &packageId, bool autoremove = false);
private slots:
    void onPackageRemoved(quint32 exit, quint32 runtime);

    // Update package
signals:
    void packageUpdated(const QString &packageName);
public slots:
    void updatePackage(const QString &packageName);
private slots:
    void onPackageUpdated(quint32 exit, quint32 runtime);

    // SSU repo actions
signals:
    void repoModified(const QString &repoAlias, RepoAction action);
    void enableReposFinished();
    void removeAllReposFinished();
public slots:
    void addRepo(const QString &author);
    void modifyRepo(const QString &repoAlias, RepoAction action);
    void enableRepos(bool enable);
    void removeAllRepos();

    // Refresh repos
public slots:
    void refreshRepo(const QString &repoAlias, bool force = false);
    void refreshRepos(bool force = false);
private slots:
    void refreshNextRepo(quint32 exit, quint32 runtime);

    // Get ORN repositories
public:
    QList<OrnRepo> repoList() const;

    // Get installed packages
signals:
    void installedPackages(const QList<OrnInstalledPackage> &packages);
public slots:
    void getInstalledPackages(const QString &packageName = QString());

private:
    explicit OrnPm(QObject *parent = nullptr);
    ~OrnPm();

    OrnPmPrivate *d_ptr;
    static OrnPm *g_instance;
};

#endif // ORNPM_H
