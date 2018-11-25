#include "ornabstractlistmodel.h"
#include "ornclient.h"

#include <QNetworkReply>
#include <QDebug>


OrnAbstractListModel::OrnAbstractListModel(bool fetchable, QObject *parent)
    : QAbstractListModel(parent)
    , mFetchable(fetchable)
    , mCanFetchMore(true)
    , mFetching(false)
    , mNetworkError(false)
    , mPage(0)
    , mApiReply(nullptr)
{
}

OrnAbstractListModel::~OrnAbstractListModel()
{
    qDeleteAll(mData);
}

bool OrnAbstractListModel::fetching() const
{
    return mFetching;
}

bool OrnAbstractListModel::networkError() const
{
    return mNetworkError;
}

void OrnAbstractListModel::reset()
{
    qDebug() << "Resetting model";
    this->beginResetModel();
    auto d = mData;
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
    // Delete data only after reset finished
    qDeleteAll(d);
}

void OrnAbstractListModel::fetch(const QString &resource, QUrlQuery query)
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
        if (doc.isArray())
        {
            this->onJsonReady(doc);
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

int OrnAbstractListModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? mData.size() : 0;
}

bool OrnAbstractListModel::canFetchMore(const QModelIndex &parent) const
{
    return !parent.isValid() ? mCanFetchMore : false;
}
