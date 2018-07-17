#include "orntagsmodel.h"
#include "orntaglistitem.h"
#include "ornapirequest.h"
#include "orn.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>


OrnTagsModel::OrnTagsModel(QObject *parent) :
    OrnAbstractAppsModel(false, parent)
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
    auto url = OrnApiRequest::apiUrl(id.prepend("tags/"));
    auto request = OrnApiRequest::networkRequest(url);
    auto reply = Orn::networkAccessManager()->get(request);
    connect(reply, &QNetworkReply::finished, this, &OrnTagsModel::onReplyFinished);
}

void OrnTagsModel::onReplyFinished()
{
    auto reply = static_cast<QNetworkReply *>(this->sender());

    if (reply->error() == QNetworkReply::NoError)
    {
        QJsonParseError error;
        auto jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error == QJsonParseError::NoError)
        {
            QJsonArray arr;
            arr.append(jsonDoc.object());
            OrnAbstractListModel::processReply<OrnTagListItem>(QJsonDocument(arr));
        }
        else
        {
            qCritical() << "Could not parse reply:" << error.errorString();
        }
    }
    else
    {
        qDebug() << "Network request error" << reply->error()
                 << "-" << reply->errorString();
    }
    reply->deleteLater();
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
