#include "orntagsmodel.h"
#include "ornclient.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>


OrnTagsModel::OrnTagsModel(QObject *parent)
    : OrnAbstractListModel(false, parent)
{}

QStringList OrnTagsModel::tagIds() const
{
    return mTagIds;
}

void OrnTagsModel::setTagIds(const QStringList &ids)
{
    if (mTagIds != ids)
    {
        mTagIds = ids;
        emit this->tagIdsChanged();
        this->reset();
    }
}

QVariant OrnTagsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    const auto &tag = mData[index.row()];
    switch (role)
    {
    case TagIdRole:
        return tag.tagId;
    case AppsCountRole:
        return tag.appsCount;
    case NameRole:
        return tag.name;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> OrnTagsModel::roleNames() const
{
    return {
        { TagIdRole,     "tagId" },
        { AppsCountRole, "appsCount" },
        { NameRole,      "name" }
    };
}

void OrnTagsModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
    {
        return;
    }

    mFetching = true;
    emit this->fetchingChanged();

    auto client = OrnClient::instance();
    auto size = mTagIds.size();
    QString resourcePrefix(QStringLiteral("tags/"));

    for (const auto &id : mTagIds)
    {
        auto request = client->apiRequest(resourcePrefix + id);
        qDebug() << "Fetching data from" << request.url().toString();
        auto reply = client->networkAccessManager()->get(request);
        connect(reply, &QNetworkReply::finished, [this, client, size, reply]()
        {
            auto doc = client->processReply(reply);
            if (doc.isObject())
            {
                mFetchedTags.append(doc.object());
                if (mFetchedTags.size() == size)
                {
                    this->processReply(QJsonDocument(mFetchedTags));
                    mFetchedTags = QJsonArray();                    
                    mFetching = false;
                    emit this->fetchingChanged();
                }
            }
        });
    }
}
