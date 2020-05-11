#ifndef ORNCATEGORYAPPSMODEL_H
#define ORNCATEGORYAPPSMODEL_H

#include "ornabstractappsmodel.h"

class OrnCategoryAppsModel : public OrnAbstractAppsModel
{
    Q_OBJECT
    Q_PROPERTY(quint32 categoryId READ categoryId WRITE setCategoryId NOTIFY categoryIdChanged)

public:
    explicit OrnCategoryAppsModel(QObject *parent = nullptr);

    quint32 categoryId() const;
    void setCategoryId(quint32 categoryId);

signals:
    void categoryIdChanged();

private:
    quint32 mCategoryId{0};

    // QAbstractItemModel interface
public:
    void fetchMore(const QModelIndex &parent) override;
};

#endif // ORNCATEGORYAPPSMODEL_H
