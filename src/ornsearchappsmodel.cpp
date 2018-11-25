#include "ornsearchappsmodel.h"


OrnSearchAppsModel::OrnSearchAppsModel(QObject *parent)
    : OrnAbstractAppsModel(true, parent)
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
