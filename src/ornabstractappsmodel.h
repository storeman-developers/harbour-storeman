#pragma once

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
        CategoryIdRole,
        VisibilityRole,
    };
    Q_ENUM(Role)

    OrnAbstractAppsModel(bool fetchable, QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
};
