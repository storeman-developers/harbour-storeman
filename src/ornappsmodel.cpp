#include "ornappsmodel.h"
#include "ornapplistitem.h"
#include "ornpm.h"
#include "ornclient.h"


OrnAppsModel::OrnAppsModel(bool fetchable, QObject *parent)
    : OrnAbstractListModel(fetchable, parent)
{
    connect(OrnPm::instance(), &OrnPm::packageStatusChanged,
            this, [this](const QString &packageName, int status)
    {
        Q_UNUSED(status)

        auto size = mData.size();
        for (size_t i = 0; i < size; ++i)
        {
            if (mData[i].package == packageName)
            {
                auto ind = this->createIndex(int(i), 0);
                emit this->dataChanged(ind, ind, {PackageStatusRole});
                return;
            }
        }
    });

    connect(OrnClient::instance(), &OrnClient::bookmarkChanged,
            this, [this](quint32 appid, bool bookmarked)
    {
        Q_UNUSED(bookmarked)

        for (size_t i = 0, size = mData.size(); i < size; ++i)
        {
            if (mData[i].appId == appid)
            {
                auto ind = this->createIndex(int(i), 0);
                emit this->dataChanged(ind, ind, {BookmarkRole});
                return;
            }
        }
    });

    connect(OrnClient::instance(), &OrnClient::categoryVisibilityChanged,
            this, [this](quint32 categoryId, bool visible)
    {
        Q_UNUSED(visible)

        for (size_t i = 0, size = mData.size(); i < size; ++i)
        {
            if (mData[i].categoryId == categoryId)
            {
                auto ind = this->createIndex(int(i), 0);
                emit this->dataChanged(ind, ind, {VisibilityRole});
                return;
            }
        }
    });
}

void OrnAppsModel::setFetchable(bool fetchable)
{
    if (mFetchable != fetchable)
    {
        mFetchable = fetchable;
        emit this->fetchableChanged();
        this->reset();
    }
}

void OrnAppsModel::setResource(const QString &resource)
{
    if (mResource != resource)
    {
        mResource = resource;
        emit this->resourceChanged();
        this->reset();
    }
}

QVariant OrnAppsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    const auto &app = mData[size_t(index.row())];
    switch (role)
    {
    case SortRole:
        return app.title.toLower();
    case ValidityRole:
        return app.valid;
    case BookmarkRole:
        return OrnClient::instance()->hasBookmark(app.appId);
    case PackageStatusRole:
        return OrnPm::instance()->packageStatus(app.package);
    case AppIdRole:
        return app.appId;
    case CreateDateRole:
        return app.createDate;
    case RatingCountRole:
        return app.ratingCount;
    case RatingRole:
        return app.rating;
    case TitleRole:
        return app.title;
    case UserNameRole:
        return app.userName;
    case IconSourceRole:
        return app.iconSource;
    case SinceUpdateRole:
        return app.sinceUpdate;
    case CategoryRole:
        return app.category;
    case CategoryIdRole:
        return app.categoryId;
    case VisibilityRole:
        return OrnClient::instance()->categoryVisible(app.categoryId);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> OrnAppsModel::roleNames() const
{
    return {
        { ValidityRole,      "isValid" },
        { BookmarkRole,      "isBookmarked" },
        { PackageStatusRole, "packageStatus" },
        { AppIdRole,         "appId" },
        { CreateDateRole,    "createDate" },
        { RatingCountRole,   "ratingCount" },
        { RatingRole,        "rating" },
        { TitleRole,         "title" },
        { UserNameRole,      "userName" },
        { IconSourceRole,    "iconSource" },
        { SinceUpdateRole,   "sinceUpdate" },
        { CategoryRole,      "category" },
        { CategoryIdRole,    "categoryId" },
        { VisibilityRole,    "visible" }
    };
}

void OrnAppsModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid() || mResource.isEmpty())
    {
        return;
    }
    OrnAbstractListModel::fetch(mResource);
}
