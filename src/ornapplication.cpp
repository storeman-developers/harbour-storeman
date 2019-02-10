#include "ornapplication.h"
#include "orncategorylistitem.h"
#include "ornclient.h"
#include "ornutils.h"

#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QDebug>


#ifdef QT_DEBUG
QDebug operator<<(QDebug dbg, const OrnApplication *app)
{
    dbg.nospace() << "OrnApplication(" << (void*)app << ", ";
    if (!app->mPackageName.isEmpty())
    {
        dbg << app->mPackageName;
    }
    else
    {
        dbg << app->mAppId;
    }
    dbg << ')';
    return dbg.space();
}
#endif

OrnApplication::OrnApplication(QObject *parent)
    : QObject(parent)
    , mRepoStatus(OrnPm::RepoUnknownStatus)
    , mPackageStatus(OrnPm::PackageUnknownStatus)
    , mAppId(0)
    , mUserId(0)
    , mRatingCount(0)
    , mCommentsCount(0)
    , mRating(0.0)
{
    auto ornPm = OrnPm::instance();
    connect(ornPm, &OrnPm::repoModified, this, &OrnApplication::onRepoListChanged);
    connect(ornPm, &OrnPm::packageStatusChanged, this, &OrnApplication::onPackageStatusChanged);
    connect(ornPm, &OrnPm::updatablePackagesChanged, this, &OrnApplication::onUpdatablePackagesChanged);

    connect(ornPm, &OrnPm::packageVersions, this, &OrnApplication::onPackageVersions);

    auto client = OrnClient::instance();
    connect(client, &OrnClient::userVoteFinished,
            [this](quint32 appId, quint32 userVote, quint32 count, float rating)
    {
        if (mAppId == appId)
        {
            mUserVote = userVote;
            mRatingCount = count;
            mRating = rating;
            emit this->ratingChanged();
        }
    });
}

quint32 OrnApplication::appId() const
{
    return mAppId;
}

void OrnApplication::setAppId(quint32 appId)
{
    if (mAppId != appId)
    {
        mAppId = appId;
        emit this->appIdChanged();
    }
}

QString OrnApplication::installedVersion() const
{
    return mInstalledVersion.version;
}

quint64 OrnApplication::installedVersionSize() const
{
    return mInstalledVersion.installSize;
}

QString OrnApplication::installedId() const
{
    if (mPackageName.isEmpty())
    {
        return QString();
    }
    return mInstalledVersion.packageId(mPackageName);
}

QString OrnApplication::availableVersion() const
{
    return mAvailableVersion.version;
}

bool OrnApplication::availableVersionIsNewer() const
{
    return mInstalledVersion < mAvailableVersion;
}

quint64 OrnApplication::availableVersionDownloadSize() const
{
    return mAvailableVersion.downloadSize;
}

quint64 OrnApplication::availableVersionInstallSize() const
{
    return mAvailableVersion.installSize;
}

QString OrnApplication::availableId() const
{
    if (mPackageName.isEmpty())
    {
        return QString();
    }
    return mAvailableVersion.packageId(mPackageName);
}

QString OrnApplication::globalVersion() const
{
    return mGlobalVersion.version;
}

bool OrnApplication::globalVersionIsNewer() const
{
    return mAvailableVersion < mGlobalVersion &&
           mInstalledVersion < mGlobalVersion;
}

quint64 OrnApplication::globalVersionDownloadSize() const
{
    return mGlobalVersion.downloadSize;
}

quint64 OrnApplication::globalVersionInstallSize() const
{
    return mGlobalVersion.installSize;
}

QString OrnApplication::category() const
{
    return mCategories.empty() ? QString() :
                                 mCategories.last().toMap().value("name").toString();
}

void OrnApplication::ornRequest()
{
    auto client = OrnClient::instance();
    auto request = client->apiRequest(QStringLiteral("apps/%0").arg(mAppId));
    qDebug() << "Fetching" << request.url().toString();
    auto reply = client->networkAccessManager()->get(request);
    connect(reply, &QNetworkReply::finished, [this, client, reply]()
    {
        auto jsonDoc = client->processReply(reply);
        if (!jsonDoc.isObject())
        {
            return;
        }
        auto jsonObject = jsonDoc.object();
        QString urlKey(QStringLiteral("url"));
        QString nameKey(QStringLiteral("name"));

        mCommentsOpen = jsonObject[QStringLiteral("comments_open")].toBool();
        mCommentsCount = OrnUtils::toUint(jsonObject[QStringLiteral("comments_count")]);
        mDownloadsCount = OrnUtils::toUint(jsonObject[QStringLiteral("downloads")]);
        mTitle = OrnUtils::toString(jsonObject[QStringLiteral("title")]);
        mIconSource = OrnUtils::toString(jsonObject[QStringLiteral("icon")].toObject()[urlKey]);
        mPackageName = OrnUtils::toString(jsonObject[QStringLiteral("package")].toObject()[nameKey]);
        mBody = OrnUtils::toString(jsonObject[QStringLiteral("body")]);
        mChangelog = OrnUtils::toString(jsonObject[QStringLiteral("changelog")]);
        if (mChangelog == QLatin1String("<p>(none)</p>"))
        {
            mChangelog.clear();
        }
        mCreated = OrnUtils::toDateTime(jsonObject[QStringLiteral("created")]);
        mUpdated = OrnUtils::toDateTime(jsonObject[QStringLiteral("updated")]);

        auto userObject = jsonObject[QStringLiteral("user")].toObject();
        mUserId = OrnUtils::toUint(userObject[QStringLiteral("uid")]);
        mUserName = OrnUtils::toString(userObject[nameKey]);
        mUserIconSource = OrnUtils::toString(userObject[QStringLiteral("picture")].toObject()[urlKey]);

        QString ratingKey(QStringLiteral("rating"));
        auto ratingObject = jsonObject[ratingKey].toObject();
        mRatingCount = OrnUtils::toUint(ratingObject[QStringLiteral("count")]);
        mUserVote = OrnUtils::toUint(ratingObject[QStringLiteral("user_vote")]);
        mRating = ratingObject[ratingKey].toString().toFloat();

        mTagIds.clear();
        QString tidKey(QStringLiteral("tid"));
        for (const QJsonValueRef id : jsonObject[QStringLiteral("tags")].toArray())
        {
            mTagIds << OrnUtils::toString(id.toObject()[tidKey]);
        }

        auto catIds = OrnUtils::toIntList(jsonObject[QStringLiteral("category")]);
        mCategories.clear();
        for (const auto &id : catIds)
        {
            mCategories << QVariantMap{
                { "id",   id },
                { "name", OrnCategoryListItem::categoryName(id) }
            };
        }

        QString thumbsKey(QStringLiteral("thumbs"));
        QString largeKey(QStringLiteral("large"));
        auto jsonArray = jsonObject[QStringLiteral("screenshots")].toArray();
        mScreenshots.clear();
        for (const QJsonValueRef v: jsonArray)
        {
            auto o = v.toObject();
            mScreenshots << QVariantMap{
                { "url",   OrnUtils::toString(o[urlKey]) },
                { "thumb", OrnUtils::toString(o[thumbsKey].toObject()[largeKey]) }
            };
        }

        if (!mUserName.isEmpty())
        {
            // Generate repository name
            mRepoAlias = OrnPm::repoNamePrefix + mUserName;
            // Update the repository and package information
            this->onRepoListChanged();
        }
        else
        {
            qCritical() << this << ": no user name in the responce - the repository and "
                                   "package information could not be updated!";
            mRepoAlias.clear();
        }

        if (!mPackageName.isEmpty())
        {
            qDebug() << this << ": information updated";
        }
        else
        {
            qWarning() << this << ": information updated but it doesn't have any package name!";
        }

        emit this->ornRequestFinished();
        emit this->commentsCountChanged();
        emit this->ratingChanged();
    });
}

void OrnApplication::onRepoListChanged()
{
    if (mRepoAlias.isEmpty())
    {
        return;
    }

    auto ornPm = OrnPm::instance();
    auto repoStatus = ornPm->repoStatus(mRepoAlias);
    if (mRepoStatus != repoStatus)
    {
        bool hasPackage = !mPackageName.isEmpty();
        qDebug() << this << ": repository" << mRepoAlias << "status changed to" << repoStatus;
        mRepoStatus = repoStatus;
        emit this->repoStatusChanged();

        if (hasPackage)
        {
            auto packageStatus = ornPm->packageStatus(mPackageName);
            if (mPackageStatus != packageStatus)
            {
                mPackageStatus = packageStatus;
                emit this->packageStatusChanged();
                this->updateDesktopFile();
            }

            ornPm->getPackageVersions(mPackageName);
        }
        else
        {
            mPackageStatus = OrnPm::PackageNotInstalled;
            emit this->packageStatusChanged();
        }
    }
}

void OrnApplication::onPackageStatusChanged(const QString &packageName, OrnPm::PackageStatus status)
{
    if (mPackageName == packageName &&
        mPackageStatus != status)
    {
        qDebug() << this << ": status changed to" << status;
        mPackageStatus = status;
        emit this->packageStatusChanged();
        OrnPm::instance()->getPackageVersions(mPackageName);
        this->updateDesktopFile();
    }
}

void OrnApplication::onUpdatablePackagesChanged()
{
    if (!mPackageName.isEmpty())
    {
        OrnPm::instance()->getPackageVersions(mPackageName);
    }
}

void OrnApplication::onPackageVersions(const QString &packageName, const OrnPackageVersionList &versions)
{
    if (mPackageName != packageName)
    {
        return;
    }

    bool availableNewer = this->availableVersionIsNewer();
    bool globalNewer    = this->globalVersionIsNewer();

    bool seekInstalled = true;
    bool seekAvailable = true;
    bool seekGlobal    = true;

    QLatin1String installed("installed");
    for (const auto &version : versions)
    {
        if (version.repoAlias == installed)
        {
            if (seekInstalled && mInstalledVersion != version)
            {
                mInstalledVersion = version;
                emit this->installedVersionChanged();
                seekInstalled = false;
            }
        }
        else if (version.repoAlias == mRepoAlias)
        {
            if (seekAvailable && mAvailableVersion != version)
            {
                mAvailableVersion = version;
                emit this->availableVersionChanged();
                if (mPackageStatus < OrnPm::PackageAvailable)
                {
                    mPackageStatus = OrnPm::PackageAvailable;
                    emit this->packageStatusChanged();
                }
                seekAvailable = false;
            }
        }
        else if (seekGlobal && mGlobalVersion != version)
        {
            mGlobalVersion = version;
            emit this->globalVersionChanged();
            seekGlobal = false;
        }
        if (!seekInstalled && !seekAvailable && !seekGlobal)
        {
            break;
        }
    }

    if (this->availableVersionIsNewer() != availableNewer)
    {
        emit this->availableVersionIsNewerChanged();
    }
    if (this->globalVersionIsNewer() != globalNewer)
    {
        emit this->globalVersionIsNewerChanged();
    }
}

void OrnApplication::updateDesktopFile()
{
    auto desktopFile = mDesktopFile;
    if (mPackageStatus == OrnPm::PackageInstalled)
    {
        desktopFile = QStandardPaths::locate(
                    QStandardPaths::ApplicationsLocation, mPackageName + ".desktop");
    }
    else
    {
        desktopFile.clear();
    }
    if (mDesktopFile != desktopFile)
    {
        if (desktopFile.size())
        {
            qDebug() << this << ": using desktop file" << desktopFile;
        }
        else
        {
            qDebug() << this << ": no desktop file was found";
        }
        mDesktopFile = desktopFile;
        emit this->desktopFileChanged();
    }
}
