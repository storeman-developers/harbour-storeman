#ifndef ORNPROXYMODEL_H
#define ORNPROXYMODEL_H

#include <QSortFilterProxyModel>

class OrnProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)

public:
    explicit OrnProxyModel(QObject *parent = nullptr);

    int limit() const;
    void setLimit(int limit);

signals:
    void limitChanged();

public:
    // Why QSortFilterProxyModel has no sort slot?
    Q_INVOKABLE void sort(Qt::SortOrder order = Qt::AscendingOrder);

private:
    int mLimit;

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    bool canFetchMore(const QModelIndex &parent) const;
};

#endif // ORNPROXYMODEL_H
