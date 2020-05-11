#pragma once

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

public slots:
    void reset();

private:
    int mLimit{-1};

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    bool canFetchMore(const QModelIndex &parent) const override;

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};
