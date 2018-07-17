#include "ornabstractlistmodel.h"
#include "ornapirequest.h"

#include <QNetworkReply>
#include <QDebug>

OrnAbstractListModel::OrnAbstractListModel(bool fetchable, QObject *parent) :
    QAbstractListModel(parent),
    mFetchable(fetchable),
    mCanFetchMore(true),
    mPage(0),
    mApiRequest(new OrnApiRequest(this))
{
    connect(mApiRequest, &OrnApiRequest::jsonReady, this, &OrnAbstractListModel::onJsonReady);
}

OrnAbstractListModel::~OrnAbstractListModel()
{
    qDeleteAll(mData);
}

OrnApiRequest *OrnAbstractListModel::apiRequest() const
{
    return mApiRequest;
}

void OrnAbstractListModel::reset()
{
    qDebug() << "Resetting model";
    this->beginResetModel();
    auto d = mData;
    mData.clear();
    mCanFetchMore = true;
    mPage = 0;
    mApiRequest->reset();
    mPrevReplyHash.clear();
    this->endResetModel();
    // Delete data only after reset finished
    qDeleteAll(d);
}

void OrnAbstractListModel::apiCall(const QString &resource, QUrlQuery query)
{
    auto url = OrnApiRequest::apiUrl(resource);
    if (mFetchable)
    {
        query.addQueryItem(QStringLiteral("page"), QString::number(mPage));
    }
    if (!query.isEmpty())
    {
        url.setQuery(query);
    }
    auto request = OrnApiRequest::networkRequest(url);
    mApiRequest->run(request);
}

int OrnAbstractListModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? mData.size() : 0;
}

bool OrnAbstractListModel::canFetchMore(const QModelIndex &parent) const
{
    return !parent.isValid() ? mCanFetchMore : false;
}
