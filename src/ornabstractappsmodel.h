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
        ValidityRole,
        BookmarkRole,
        PackageStatusRole,
        AppIdRole,
        CreateDateRole,
        RatingCountRole,
        RatingRole,
        TitleRole,
        UserNameRole,
        IconSourceRole,
        SinceUpdateRole,
        CategoryRole,
    };
    Q_ENUM(Role)

    OrnAbstractAppsModel(bool fetchable, QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
};

#endif // ORNABSTRACTAPPSMODEL_H
