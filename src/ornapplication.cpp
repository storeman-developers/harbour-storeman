#include "ornapplication.h"
#include "orncategorylistitem.h"
#include "ornclient.h"
#include "ornutils.h"
#include "ornconst.h"

#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

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
{
    auto ornPm = OrnPm::instance();
    connect(ornPm, &OrnPm::repoModified,             this, &OrnApplication::onRepoListChanged);
    connect(ornPm, &OrnPm::packageStatusChanged,     this, &OrnApplication::onPackageStatusChanged);
    connect(ornPm, &OrnPm::updatablePackagesChanged, this, &OrnApplication::onUpdatablePackagesChanged);
    connect(ornPm, &OrnPm::packageVersions,          this, &OrnApplication::onPackageVersions);

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
    return mCategories.empty()
            ? QString()
            : mCategories.last().toMap().value(OrnConst::name).toString();
}

void OrnApplication::ornRequest()
{
    auto client   = OrnClient::instance();
    auto resource = QString::number(mAppId).prepend("apps/");
    auto request  = client->apiRequest(resource);
    qDebug() << "Fetching" << request.url().toString();
    auto reply = client->networkAccessManager()->get(request);
    connect(reply, &QNetworkReply::finished, [this, client, reply]()
    {
        auto jsonDoc = client->processReply(reply);
        if (!jsonDoc.isObject())
        {
            return;
        }

        auto data       = jsonDoc.object();
        mCommentsOpen   = data[OrnConst::commentsOpen].toBool();
        mCommentsCount  = OrnUtils::toUint(data[OrnConst::commentsCount]);
        mDownloadsCount = OrnUtils::toUint(data[OrnConst::downloads]);
        mTitle          = OrnUtils::toString(data[OrnConst::title]);
        mIconSource     = OrnUtils::toString(data[OrnConst::icon].toObject()[OrnConst::url]);
        mPackageName    = OrnUtils::toString(data[OrnConst::package].toObject()[OrnConst::name]);
        mBody           = OrnUtils::toString(data[OrnConst::body]);
        mCreated        = OrnUtils::toDateTime(data[OrnConst::created]);
        mUpdated        = OrnUtils::toDateTime(data[OrnConst::updated]);
        mChangelog      = OrnUtils::toString(data[OrnConst::changelog]);
        if (mChangelog == QLatin1String{"<p>(none)</p>"})
        {
            mChangelog.clear();
        }

        auto user       = data[OrnConst::user].toObject();
        mUserId         = OrnUtils::toUint(user[OrnConst::uid]);
        mUserName       = OrnUtils::toString(user[OrnConst::name]);
        mUserIconSource = OrnUtils::toString(user[OrnConst::picture].toObject()[OrnConst::url]);

        auto rating     = data[OrnConst::rating].toObject();
        mRatingCount    = OrnUtils::toUint(rating[OrnConst::count]);
        mUserVote       = OrnUtils::toUint(rating[OrnConst::userVote]);
        mRating         = rating[OrnConst::rating].toString().toFloat();

        mTagIds.clear();
        const auto tags = data[OrnConst::tags].toArray();
        for (const QJsonValue &id : tags)
        {
            mTagIds << OrnUtils::toString(id.toObject()[OrnConst::tid]);
        }

        auto catIds     = OrnUtils::toIntList(data[OrnConst::category]);
        mCategories.clear();
        for (const auto &id : catIds)
        {
            mCategories << QVariantMap{
                { OrnConst::id,   id },
                { OrnConst::name, OrnCategoryListItem::categoryName(id) },
            };
        }

        auto screenshots = data[OrnConst::screenshots].toArray();
        mScreenshots.clear();
        for (const QJsonValueRef v: screenshots)
        {
            auto o = v.toObject();
            mScreenshots << QVariantMap{
                { OrnConst::url,   OrnUtils::toString(o[OrnConst::url]) },
                { OrnConst::thumb, OrnUtils::toString(o[OrnConst::thumbs].toObject()[OrnConst::large]) }
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
        // Pass continuous actions
        if (status < OrnPm::PackageInstalling)
        {
            // TODO: Try to avoid using QTimer here
            QTimer::singleShot(500, [this]() {
                OrnPm::instance()->getPackageVersions(mPackageName);
                this->updateDesktopFile();
            });
        }
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

    auto availableNewer = this->availableVersionIsNewer();
    auto globalNewer    = this->globalVersionIsNewer();

    auto installedVersion = mInstalledVersion;
    auto availableVersion = mAvailableVersion;
    auto globalVersion    = mGlobalVersion;

    mInstalledVersion.clear();
    mAvailableVersion.clear();
    mGlobalVersion.clear();

    bool seekInstalled = true;
    bool seekAvailable = true;
    bool seekGlobal    = true;

    for (const auto &version : versions)
    {
        if (seekInstalled && version.repoAlias == OrnConst::installed)
        {
            mInstalledVersion = version;
            seekInstalled = false;
        }
        else if (seekAvailable && version.repoAlias == mRepoAlias)
        {
            mAvailableVersion = version;
            seekAvailable = false;
        }
        else if (seekGlobal)
        {
            mGlobalVersion = version;
            seekGlobal = false;
        }
        if (!seekInstalled && !seekAvailable && !seekGlobal)
        {
            break;
        }
    }

    if (mInstalledVersion != installedVersion)
    {
        emit this->installedVersionChanged();
    }
    if (mAvailableVersion != availableVersion)
    {
        emit this->availableVersionChanged();
    }
    if (mGlobalVersion != globalVersion)
    {
        emit this->globalVersionChanged();
    }

    if (!mAvailableVersion.version.isEmpty() && mPackageStatus < OrnPm::PackageAvailable)
    {
        mPackageStatus = OrnPm::PackageAvailable;
        emit this->packageStatusChanged();
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
    auto desktopFile = mPackageStatus == OrnPm::PackageInstalled
        ? OrnUtils::desktopFile(mPackageName)
        : QString();

    if (mDesktopFile != desktopFile)
    {
        if (!desktopFile.isEmpty())
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
