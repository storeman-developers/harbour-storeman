#include "ornproxymodel.h"

OrnProxyModel::OrnProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , mLimit(-1)
{
}

int OrnProxyModel::limit() const
{
    return mLimit;
}

void OrnProxyModel::setLimit(int limit)
{
    if (mLimit != limit)
    {
        mLimit = limit;
        emit this->limitChanged();
    }
    this->invalidate();
}

void OrnProxyModel::sort(Qt::SortOrder order)
{
    QSortFilterProxyModel::sort(0, order);
}

int OrnProxyModel::rowCount(const QModelIndex &parent) const
{
    auto count = QSortFilterProxyModel::rowCount(parent);
    return mLimit > -1 ? std::min(count, mLimit) : count;
}

bool OrnProxyModel::canFetchMore(const QModelIndex &parent) const
{
    auto canFetch = QSortFilterProxyModel::canFetchMore(parent);
    return mLimit > -1 ?
                QSortFilterProxyModel::rowCount(parent) < mLimit && canFetch :
                canFetch;
}
