#include "orncommentsmodel.h"
#include "ornapirequest.h"
#include "orncommentlistitem.h"
#include "orn.h"

#include <QNetworkReply>
#include <QDebug>

OrnCommentsModel::OrnCommentsModel(QObject *parent)
    : OrnAbstractListModel(false, parent)
{}

quint32 OrnCommentsModel::appId() const
{
    return mAppId;
}

void OrnCommentsModel::setAppId(const quint32 &appId)
{
    if (mAppId != appId)
    {
        mAppId = appId;
        emit this->appIdChanged();
        this->reset();
    }
}

int OrnCommentsModel::findItemRow(const quint32 &cid) const
{
    QObjectList::size_type i = 0;
    for (const auto &c: mData)
    {
        if (static_cast<OrnCommentListItem *>(c)->commentId == cid)
        {
            return i;
        }
        ++i;
    }
    return -1;
}

void OrnCommentsModel::addComment(const quint32 &cid)
{
    auto reply = this->fetchComment(cid);
    connect(reply, &QNetworkReply::finished, [=]()
    {
        auto jsonObject = this->processReply(reply);
        if (jsonObject.isEmpty())
        {
            return;
        }
        this->beginInsertRows(QModelIndex(), 0, 0);
        mData.prepend(new OrnCommentListItem(jsonObject));
        this->endInsertRows();
    });
}

void OrnCommentsModel::editComment(const quint32 &cid)
{
    auto reply = this->fetchComment(cid);
    connect(reply, &QNetworkReply::finished, [=]()
    {
        auto jsonObject = this->processReply(reply);
        if (jsonObject.isEmpty())
        {
            return;
        }
        auto cid = Orn::toUint(jsonObject[QStringLiteral("cid")]);
        auto size = mData.size();
        for (int i = 0; i < size; ++i)
        {
            auto comment = static_cast<OrnCommentListItem *>(mData[i]);
            if (comment->commentId == cid)
            {
                comment->text = Orn::toString(jsonObject[QStringLiteral("text")]);
                auto index = this->createIndex(i, 0);
                emit this->dataChanged(index, index);
                return;
            }
        }
        qWarning() << "Could not find comment in model with such id" << cid;
    });
}

QNetworkReply *OrnCommentsModel::fetchComment(const quint32 &cid)
{
    // FIXME: need refactoring
    auto url = OrnApiRequest::apiUrl(QStringLiteral("comments/%0").arg(cid));
    auto request = OrnApiRequest::networkRequest(url);
    qDebug() << "Fetching data from" << url.toString();
    return Orn::networkAccessManager()->get(request);
}

QJsonObject OrnCommentsModel::processReply(QNetworkReply *reply)
{
    // FIXME: need refactoring
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Network request error" << reply->error()
                 << "-" << reply->errorString();
        reply->deleteLater();
        return QJsonObject();
    }

    QJsonParseError error;
    auto jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error == QJsonParseError::NoError)
    {
        return jsonDoc.object();
    }
    else
    {
        qCritical() << "Could not parse reply:" << error.errorString();
    }
    reply->deleteLater();
    return QJsonObject();
}

QVariant OrnCommentsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    auto comment = static_cast<OrnCommentListItem *>(mData[index.row()]);
    switch (role)
    {
    case CommentIdRole:
        return comment->commentId;
    case ParentIdRole:
        return comment->parentId;
    case CreatedRole:
        return comment->created;
    case UserIdRole:
        return comment->userId;
    case UserNameRole:
        return comment->userName;
    case ParentUserNameRole:
    {
        auto row = this->findItemRow(comment->parentId);
        return row == -1 ? QString() : static_cast<OrnCommentListItem *>(mData[row])->userName;
    }
    case UserIconSourceRole:
        return comment->userIconSource;
    case TextRole:
        return comment->text;
    default:
        return QVariant();
    }
}

void OrnCommentsModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
    {
        return;
    }
    OrnAbstractListModel::apiCall(QStringLiteral("apps/%0/comments").arg(mAppId));
}

QHash<int, QByteArray> OrnCommentsModel::roleNames() const
{
    return {
        { CommentIdRole,      "commentId" },
        { ParentIdRole,       "parentId" },
        { CreatedRole,        "created" },
        { UserIdRole,         "userId" },
        { UserNameRole,       "userName" },
        { ParentUserNameRole, "parentUserName" },
        { UserIconSourceRole, "userIconSource" },
        { TextRole,           "text" }
    };
}

//bool OrnCommentsModel::removeRows(int row, int count, const QModelIndex &parent)
//{
//    if (parent.isValid())
//    {
//        return false;
//    }
//    auto last = row + count;
//    this->beginRemoveRows(parent, row, last - 1);
//    QObjectList list;
//    for (auto i = row; i < last; ++i)
//    {
//        list << mData[i];
//        mData.removeAt(i);
//    }
//    this->endRemoveRows();
//    qDeleteAll(list);
//    return true;
//}

void OrnCommentsModel::onJsonReady(const QJsonDocument &jsonDoc)
{
    OrnAbstractListModel::processReply<OrnCommentListItem>(jsonDoc);
}

