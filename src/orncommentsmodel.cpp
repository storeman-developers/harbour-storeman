#include "orncommentsmodel.h"
#include "ornclient.h"
#include "ornutils.h"

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
                mData.emplace_front(jsonObject);
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
                auto cid = OrnUtils::toUint(jsonObject[QStringLiteral("cid")]);
                auto size = mData.size();
                for (size_t i = 0; i < size; ++i)
                {
                    auto &comment = mData[i];
                    if (comment.commentId == cid)
                    {
                        comment.text = OrnUtils::toString(jsonObject[QStringLiteral("text")]);
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
            // FIXME
            // Find all comments from the tree to remove
            std::set<quint32> ids = {cid};
            std::set<int> inds;
            for (int i = mData.size() - 1; i >= 0; --i)
            {
                const auto &comment = mData[i];
                if (ids.find(comment.parentId) != ids.end())
                {
                    ids.insert(comment.commentId);
                    inds.insert(i);
                }
                else if (comment.commentId == cid)
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
                        mData.erase(mData.begin() + i);
                    }
                    this->endRemoveRows();
                    last  = current;
                    first = current;
                }
            }
            this->beginRemoveRows(invalidIndex, first, last);
            for (int i = last; i >= first; --i)
            {
                mData.erase(mData.begin() + i);
            }
            mData.shrink_to_fit();
            this->endRemoveRows();
        }
    });
}

quint32 OrnCommentsModel::appId() const
{
    return mAppId;
}

void OrnCommentsModel::setAppId(quint32 appId)
{
    if (mAppId != appId)
    {
        mAppId = appId;
        emit this->appIdChanged();
        this->reset();
    }
}

int OrnCommentsModel::findItemRow(quint32 cid) const
{
    QObjectList::size_type i = 0;
    for (const auto &c : mData)
    {
        if (c.commentId == cid)
        {
            return i;
        }
        ++i;
    }
    return -1;
}

QNetworkReply *OrnCommentsModel::fetchComment(quint32 cid)
{
    auto client = OrnClient::instance();
    auto request = client->apiRequest(QStringLiteral("comments/%0").arg(cid));
    qDebug() << "Fetching data from" << request.url().toString();
    return client->networkAccessManager()->get(request);
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

    const auto &comment = mData[index.row()];
    switch (role)
    {
    case CommentIdRole:
        return comment.commentId;
    case ParentIdRole:
        return comment.parentId;
    case CreatedRole:
        return comment.created;
    case UserIdRole:
        return comment.userId;
    case UserNameRole:
        return comment.userName;
    case ParentUserNameRole:
    {
        auto row = this->findItemRow(comment.parentId);
        return row == -1 ? QString() : mData[row].userName;
    }
    case UserIconSourceRole:
        return comment.userIconSource;
    case TextRole:
        return comment.text;
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
    OrnAbstractListModel::fetch(QStringLiteral("apps/%0/comments").arg(mAppId));
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
