#include "orncommentsmodel.h"
#include "ornclient.h"
#include "orncommentlistitem.h"
#include "orn.h"

#include <QNetworkReply>
#include <QDebug>

#include <set>


OrnCommentsModel::OrnCommentsModel(QObject *parent)
    : OrnAbstractListModel(false, parent)
    , mAppId(0)
{
    connect(OrnClient::instance(), &OrnClient::commentActionFinished,
            [this](OrnClient::CommentAction action, quint32 appId, quint32 cid)
    {
        if (mAppId == 0 && mAppId != appId)
        {
            return;
        }
        if (action == OrnClient::CommentAdded)
        {
            auto reply = this->fetchComment(cid);
            connect(reply, &QNetworkReply::finished, [this, reply]()
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
        else if (action == OrnClient::CommentEdited)
        {
            auto reply = this->fetchComment(cid);
            connect(reply, &QNetworkReply::finished, [this, reply]()
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
        else if (action == OrnClient::CommentDeleted)
        {
            // Find all comments from the tree to remove
            std::set<quint32> ids = {cid};
            std::set<int> inds;
            for (int i = mData.size() - 1; i >= 0; --i)
            {
                auto comment = static_cast<OrnCommentListItem *>(mData[i]);
                if (ids.find(comment->parentId) != ids.end())
                {
                    ids.insert(comment->commentId);
                    inds.insert(i);
                }
                else if (comment->commentId == cid)
                {
                    inds.insert(i);
                }
            }
            // Try to remove comments by groups
            QModelIndex invalidIndex;
            auto iter = inds.crbegin();
            auto end  = inds.crend();
            int last  = *iter;
            int first = last;
            ++iter;
            while (iter != end)
            {
                auto current = *iter;
                ++iter;
                if (first - current == 1)
                {
                    --first;
                }
                else
                {
                    this->beginRemoveRows(invalidIndex, first, last);
                    for (int i = last; i >= first; --i)
                    {
                        mData.removeAt(i);
                    }
                    this->endRemoveRows();
                    last  = current;
                    first = current;
                }
            }
            this->beginRemoveRows(invalidIndex, first, last);
            for (int i = last; i >= first; --i)
            {
                mData.removeAt(i);
            }
            this->endRemoveRows();
        }
    });
}

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

void OrnCommentsModel::onJsonReady(const QJsonDocument &jsonDoc)
{
    OrnAbstractListModel::processReply<OrnCommentListItem>(jsonDoc);
}

