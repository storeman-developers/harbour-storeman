#include "orntagsmodel.h"
#include "orntaglistitem.h"
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

void OrnTagsModel::addTag(QString id)
{
    qDebug() << "Adding tag" << id << "to tags model";
    auto client = OrnClient::instance();
    auto request = client->apiRequest(id.prepend("tags/"));
    auto reply = client->networkAccessManager()->get(request);
    connect(reply, &QNetworkReply::finished, [this, client, reply]()
    {
        auto doc = client->processReply(reply);
        if (doc.isObject())
        {
            QJsonArray arr({doc.object()});
            this->onJsonReady(QJsonDocument(arr));
        }
    });
}

QVariant OrnTagsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    auto tag = static_cast<OrnTagListItem *>(mData[index.row()]);
    switch (role)
    {
    case TagIdRole:
        return tag->tagId;
    case AppsCountRole:
        return tag->appsCount;
    case NameRole:
        return tag->name;
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

    for (const auto &id : mTagIds)
    {
        this->addTag(id);
    }
}

void OrnTagsModel::onJsonReady(const QJsonDocument &jsonDoc)
{
    OrnAbstractListModel::processReply<OrnTagListItem>(jsonDoc);
}
