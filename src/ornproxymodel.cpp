#include "ornproxymodel.h"

OrnProxyModel::OrnProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{

}

void OrnProxyModel::sort(Qt::SortOrder order)
{
    QSortFilterProxyModel::sort(0, order);
}
