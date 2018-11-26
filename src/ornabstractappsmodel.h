#ifndef ORNABSTRACTAPPSMODEL_H
#define ORNABSTRACTAPPSMODEL_H

#include "ornabstractlistmodel.h"
#include "ornapplistitem.h"

class OrnAbstractAppsModel : public OrnAbstractListModel<OrnAppListItem>
{
    Q_OBJECT

public:

    enum Role
    {
        SortRole = Qt::UserRole,
        PackageStatusRole,
        AppIdRole,
        CreateDateRole,
        RatingCountRole,
        RatingRole,
        TitleRole,
        UserNameRole,
        IconSourceRole,
        SinceUpdateRole,
        CategoryRole
    };
    Q_ENUM(Role)

    OrnAbstractAppsModel(bool fetchable, QObject *parent = nullptr);

private slots:
    void onPackageStatusChanged(const QString &packageName, int status);

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
};

#endif // ORNABSTRACTAPPSMODEL_H
