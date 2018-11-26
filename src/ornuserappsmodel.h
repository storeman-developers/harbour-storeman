#ifndef ORNUSERAPPSMODEL_H
#define ORNUSERAPPSMODEL_H

#include "ornabstractappsmodel.h"

class OrnUserAppsModel : public OrnAbstractAppsModel
{
    Q_OBJECT
    Q_PROPERTY(quint32 userId READ userId WRITE setUserId NOTIFY userIdChanged)

public:
    explicit OrnUserAppsModel(QObject *parent = nullptr);

    quint32 userId() const;
    void setUserId(quint32 userId);

signals:
    void userIdChanged();

private:
    quint32 mUserId;

    // QAbstractItemModel interface
public:
    void fetchMore(const QModelIndex &parent);
};

#endif // ORNUSERAPPSMODEL_H
