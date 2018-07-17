#ifndef ORNPROXYMODEL_H
#define ORNPROXYMODEL_H

#include <QSortFilterProxyModel>

class OrnProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit OrnProxyModel(QObject *parent = nullptr);

public:
    // Why QSortFilterProxyModel has no sort slot?
    Q_INVOKABLE void sort(Qt::SortOrder order = Qt::AscendingOrder);
};

#endif // ORNPROXYMODEL_H
