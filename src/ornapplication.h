#ifndef ORNAPPLICATION_H
#define ORNAPPLICATION_H

#include "ornpm.h"
#include "ornpackageversion.h"

#include <QDateTime>

class OrnApplication : public QObject
{
    friend class OrnBookmarksModel;

#ifdef QT_DEBUG
    friend QDebug operator<<(QDebug dbg, const OrnApplication *app);
#endif

    Q_OBJECT

    Q_PROPERTY(OrnPm::RepoStatus repoStatus MEMBER mRepoStatus NOTIFY repoStatusChanged)
    Q_PROPERTY(OrnPm::PackageStatus packageStatus MEMBER mPackageStatus NOTIFY packageStatusChanged)
    Q_PROPERTY(QString repoAlias MEMBER mRepoAlias NOTIFY ornRequestFinished)
    Q_PROPERTY(QString desktopFile MEMBER mDesktopFile NOTIFY desktopFileChanged)
    Q_PROPERTY(QString installedVersion READ installedVersion NOTIFY installedVersionChanged)
    Q_PROPERTY(quint64 installedVersionSize READ installedVersionSize NOTIFY installedVersionChanged)
    Q_PROPERTY(QString installedId READ installedId NOTIFY installedVersionChanged)
    Q_PROPERTY(QString availableVersion READ availableVersion NOTIFY availableVersionChanged)
    Q_PROPERTY(bool availableVersionIsNewer READ availableVersionIsNewer NOTIFY availableVersionIsNewerChanged)
    Q_PROPERTY(quint64 availableVersionDownloadSize READ availableVersionDownloadSize NOTIFY availableVersionChanged)
    Q_PROPERTY(quint64 availableVersionInstallSize READ availableVersionInstallSize NOTIFY availableVersionChanged)
    Q_PROPERTY(QString availableId READ availableId NOTIFY availableVersionChanged)
    Q_PROPERTY(QString globalVersion READ globalVersion NOTIFY globalVersionChanged)
    Q_PROPERTY(bool globalVersionIsNewer READ globalVersionIsNewer NOTIFY globalVersionIsNewerChanged)
    Q_PROPERTY(quint64 globalVersionDownloadSize READ globalVersionDownloadSize NOTIFY globalVersionChanged)
    Q_PROPERTY(quint64 globalVersionInstallSize READ globalVersionInstallSize NOTIFY globalVersionChanged)

    Q_PROPERTY(quint32 appId READ appId WRITE setAppId NOTIFY appIdChanged)
    Q_PROPERTY(quint32 userId MEMBER mUserId NOTIFY ornRequestFinished)
    Q_PROPERTY(quint32 ratingCount MEMBER mRatingCount NOTIFY ratingChanged)
    Q_PROPERTY(quint32 userVote MEMBER mUserVote NOTIFY ratingChanged)
    Q_PROPERTY(bool commentsOpen MEMBER mCommentsOpen NOTIFY ornRequestFinished)
    Q_PROPERTY(quint32 commentsCount MEMBER mCommentsCount NOTIFY commentsCountChanged)
    Q_PROPERTY(quint32 downloadsCount MEMBER mDownloadsCount NOTIFY ornRequestFinished)
    Q_PROPERTY(float rating MEMBER mRating NOTIFY ratingChanged)
    Q_PROPERTY(QString title MEMBER mTitle NOTIFY ornRequestFinished)
    Q_PROPERTY(QString userName MEMBER mUserName NOTIFY ornRequestFinished)
    Q_PROPERTY(QString userIconSource MEMBER mUserIconSource NOTIFY ornRequestFinished)
    Q_PROPERTY(QString iconSource MEMBER mIconSource NOTIFY ornRequestFinished)
    Q_PROPERTY(QString packageName MEMBER mPackageName NOTIFY ornRequestFinished)
    Q_PROPERTY(QString body MEMBER mBody NOTIFY ornRequestFinished)
    Q_PROPERTY(QString changelog MEMBER mChangelog NOTIFY ornRequestFinished)
    Q_PROPERTY(QString category READ category NOTIFY ornRequestFinished)
    Q_PROPERTY(QDateTime created MEMBER mCreated NOTIFY ornRequestFinished)
    Q_PROPERTY(QDateTime ornRequestFinished MEMBER mUpdated NOTIFY ornRequestFinished)
    Q_PROPERTY(QStringList tagIds MEMBER mTagIds NOTIFY ornRequestFinished)
    Q_PROPERTY(QVariantList categories MEMBER mCategories NOTIFY ornRequestFinished)
    Q_PROPERTY(QVariantList screenshots MEMBER mScreenshots NOTIFY ornRequestFinished)

public:

    explicit OrnApplication(QObject *parent = nullptr);

    quint32 appId() const;
    void setAppId(quint32 appId);

    QString installedVersion() const;
    quint64 installedVersionSize() const;
    QString installedId() const;

    QString availableVersion() const;
    bool availableVersionIsNewer() const;
    quint64 availableVersionDownloadSize() const;
    quint64 availableVersionInstallSize() const;
    QString availableId() const;

    QString globalVersion() const;
    bool globalVersionIsNewer() const;
    quint64 globalVersionDownloadSize() const;
    quint64 globalVersionInstallSize() const;

    QString category() const;

signals:
    void appIdChanged();
    void ornRequestFinished();
    void desktopFileChanged();
    void repoStatusChanged();
    void packageStatusChanged();
    void installedVersionChanged();
    void availableVersionChanged();
    void availableVersionIsNewerChanged();
    void globalVersionChanged();
    void globalVersionIsNewerChanged();
    void commentsCountChanged();
    void ratingChanged();

public slots:
    void ornRequest();

private slots:
    void onRepoListChanged();
    void onPackageStatusChanged(const QString &packageName, OrnPm::PackageStatus status);
    void onUpdatablePackagesChanged();
    void onPackageVersions(const QString &packageName, const OrnPackageVersionList &versions);

private:
    void updateDesktopFile();

    OrnPm::RepoStatus mRepoStatus;
    OrnPm::PackageStatus mPackageStatus;

    bool mCommentsOpen;
    quint32 mAppId;
    quint32 mUserId;
    quint32 mRatingCount;
    quint32 mUserVote;
    quint32 mCommentsCount;
    quint32 mDownloadsCount;

    float mRating;

    OrnPackageVersion mInstalledVersion;
    OrnPackageVersion mAvailableVersion;
    OrnPackageVersion mGlobalVersion;

    QString mRepoAlias;
    QString mDesktopFile;

    QString mTitle;
    QString mUserName;
    QString mUserIconSource;
    QString mIconSource;
    QString mPackageName;
    QString mBody;
    QString mChangelog;
    QDateTime mCreated;
    QDateTime mUpdated;
    QStringList mTagIds;
    /// A list of maps with keys [ id, name ]
    QVariantList mCategories;
    /// A list of maps with keys [ url, thumb ]
    QVariantList mScreenshots;
};

#endif // ORNAPPLICATION_H
