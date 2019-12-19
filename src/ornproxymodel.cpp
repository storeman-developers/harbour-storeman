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

bool OrnProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    auto role = this->filterRole();
    // The default value
    if (role == Qt::DisplayRole)
    {
        return true;
    }

    auto model = this->sourceModel();
    auto idx = model->index(source_row, 0, source_parent);
    auto dat = model->data(idx, role);
#ifdef QT_DEBUG
    if (dat.type() != QVariant::Bool)
    {
        qWarning("The filter role must return boolean!");
    }
#endif
    return dat.toBool();
}
