#pragma once

#include "ornabstractlistmodel.h"
#include "ornapplistitem.h"

class OrnAppsModel : public OrnAbstractListModel<OrnAppListItem>
{
    Q_OBJECT
    Q_PROPERTY(bool    fetchable READ fetchable WRITE setFetchable NOTIFY fetchableChanged)
    Q_PROPERTY(QString resource  READ resource  WRITE setResource  NOTIFY resourceChanged)

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

    OrnAppsModel(bool fetchable = true, QObject *parent = nullptr);

    bool fetchable() const
    { return mFetchable; }

    void setFetchable(bool fetchable);

    QString resource() const
    { return mResource; }

    void setResource(const QString &resource);

signals:
    void fetchableChanged();
    void resourceChanged();

private:
    QString mResource;

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void fetchMore(const QModelIndex &parent) override;
};
