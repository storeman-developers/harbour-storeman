#ifndef ORNABSTRACTAPPSMODEL_H
#define ORNABSTRACTAPPSMODEL_H

#include "ornabstractlistmodel.h"

class OrnAbstractAppsModel : public OrnAbstractListModel
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
    void onPackageStatusChanged(const QString &packageName, const int &status);

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    // OrnAbstractListModel interface
protected:
    void onJsonReady(const QJsonDocument &jsonDoc);
};

#endif // ORNABSTRACTAPPSMODEL_H
