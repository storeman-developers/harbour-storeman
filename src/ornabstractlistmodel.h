#ifndef ORNABSTRACTLISTMODEL_H
#define ORNABSTRACTLISTMODEL_H

#include "ornclient.h"

#include <QAbstractListModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QNetworkReply>

#include <QDebug>

#include <deque>


class OrnAbstractListModelBase : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool fetching READ fetching NOTIFY fetchingChanged)
    Q_PROPERTY(bool networkError READ networkError NOTIFY networkErrorChanged)

public:
    OrnAbstractListModelBase(QObject *parent = nullptr)
        : QAbstractListModel(parent)
        , mFetching(false)
        , mNetworkError(false)
    {}

    inline bool fetching() const { return mFetching; }
    bool networkError() const { return mNetworkError; }

public slots:
    void reset() { this->resetImpl(); }

signals:
    void fetchingChanged();
    void networkErrorChanged();

protected:
    virtual void resetImpl() = 0;

protected:
    bool mFetching;
    bool mNetworkError;
};

template<typename T>
class OrnAbstractListModel : public OrnAbstractListModelBase
{
public:
    OrnAbstractListModel(bool fetchable, QObject *parent = nullptr)
        : OrnAbstractListModelBase(parent)
        , mFetchable(fetchable)
        , mCanFetchMore(true)
        , mPage(0)
        , mApiReply(nullptr)
    {}

protected:
    void resetImpl() final
    {
        qDebug() << "Resetting model";
        this->beginResetModel();
        mData.clear();
        mCanFetchMore = true;
        mPage = 0;
        mPrevReplyHash.clear();
        if (mApiReply)
        {
            mApiReply->deleteLater();
            mApiReply = nullptr;
        }
        if (mFetching)
        {
            mFetching = false;
            emit this->fetchingChanged();
        }
        if (mNetworkError)
        {
            mNetworkError = false;
            emit this->networkErrorChanged();
        }
        this->endResetModel();
    }

    void fetch(const QString &resource, QUrlQuery query = QUrlQuery())
    {
        if (mFetching)
        {
            qWarning() << this << "Model is already fetching data";
            return;
        }
        mFetching = true;
        emit this->fetchingChanged();
        if (mFetchable)
        {
            query.addQueryItem(QStringLiteral("page"), QString::number(mPage));
        }
        auto client = OrnClient::instance();
        auto request = client->apiRequest(resource, query);
        qDebug() << "Fetching data from" << request.url().toString();
        mApiReply = client->networkAccessManager()->get(request);
        connect(mApiReply, &QNetworkReply::finished, [this, client]()
        {
            auto doc = client->processReply(mApiReply);
            mApiReply = nullptr;
            if (doc.isArray())
            {
                this->processReply(doc);
            }
            else
            {
                mCanFetchMore = false;
                mNetworkError = true;
                emit this->networkErrorChanged();
                mFetching = false;
                emit this->fetchingChanged();
            }
        });
    }
    virtual void processReply(const QJsonDocument &jsonDoc)
    {
        auto jsonArray = jsonDoc.array();
        if (jsonArray.size() > 0)
        {
            if (!mFetchable)
            {
                mCanFetchMore = false;
            }
            else
            {
                // An ugly patch for some models with repeating data (search model)
                auto replyHash = QCryptographicHash::hash(
                            jsonDoc.toJson(), QCryptographicHash::Md5);
                if (mPrevReplyHash == replyHash)
                {
                    qDebug() << "Current reply is equal to the previous one. "
                                "Considering the model has fetched all data";
                    mCanFetchMore = false;
                    return;
                }
                mPrevReplyHash = replyHash;
            }
            auto oldSize = mData.size();
            auto appendSize = jsonArray.size();
            this->beginInsertRows(QModelIndex(), oldSize, oldSize + appendSize - 1);
            for (const QJsonValueRef jsonValue : jsonArray)
            {
                // Each class of list item should implement a constructor
                // SomeListItem(const QJsonObject &)
                mData.emplace_back(jsonValue.toObject());
            }
            ++mPage;
            this->endInsertRows();
            qDebug() << appendSize << "item(s) have been added to the model";
        }
        else
        {
            qDebug() << "Reply is empty, the model has fetched all data";
            mCanFetchMore = false;
        }
        mFetching = false;
        emit this->fetchingChanged();
    }

protected:
    bool     mFetchable;
    bool     mCanFetchMore;
    quint32  mPage;
    std::deque<T> mData;
    QNetworkReply *mApiReply;

private:
    QByteArray mPrevReplyHash;

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const
    {
        return !parent.isValid() ? mData.size() : 0;
    }
    virtual void fetchMore(const QModelIndex &parent) = 0;
    bool canFetchMore(const QModelIndex &parent) const
    {
        return !parent.isValid() ? mCanFetchMore : false;
    }
};

#endif // ORNABSTRACTLISTMODEL_H
