#include "ornsearchappsmodel.h"

#include <QCryptographicHash>


OrnSearchAppsModel::OrnSearchAppsModel(QObject *parent)
    : OrnAppsModel(true, parent)
{
    mCanFetchMore = false;
}

QString OrnSearchAppsModel::searchKey() const
{
    return mSearchKey;
}

void OrnSearchAppsModel::setSearchKey(const QString &searchKey)
{
    if (mSearchKey != searchKey)
    {
        mSearchKey = searchKey;
        emit this->searchKeyChanged();
        this->reset();
        mCanFetchMore = !mSearchKey.isEmpty();
    }
}

void OrnSearchAppsModel::resetImpl()
{
    mPrevReplyHash.clear();
    OrnAbstractListModel::resetImpl();
}

void OrnSearchAppsModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
    {
        return;
    }
    if (mSearchKey.isEmpty())
    {
        qWarning() << "Could not search with an empty search key";
        return;
    }
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("keys"), mSearchKey);
    OrnAbstractListModel::fetch(QStringLiteral("search/apps"), query);
}

void OrnSearchAppsModel::processReply(const QJsonDocument &jsonDoc)
{
    // An ugly patch for srepeating data
    auto replyHash = QCryptographicHash::hash(
                jsonDoc.toJson(), QCryptographicHash::Md5);
    if (mPrevReplyHash == replyHash)
    {
        qDebug() << "Current reply is equal to the previous one. "
                    "Considering the model has fetched all data";
        mCanFetchMore = false;
    }
    else
    {
        mPrevReplyHash = replyHash;
    }
    OrnAbstractListModel::processReply(jsonDoc);
}
