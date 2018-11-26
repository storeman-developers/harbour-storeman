#ifndef ORNCATEGORIESMODEL_H
#define ORNCATEGORIESMODEL_H

#include "ornabstractlistmodel.h"
#include "orncategorylistitem.h"

/**
 * @brief The categories model class
 * This will work properly only if api response contains sorted categories
 */
class OrnCategoriesModel : public OrnAbstractListModel<OrnCategoryListItem>
{
    Q_OBJECT
public:
    enum Role
    {
        CategoryIdRole = Qt::UserRole,
        AppsCountRole,
        DepthRole,
        NameRole,
    };
    Q_ENUM(Role)

    explicit OrnCategoriesModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const;
    void fetchMore(const QModelIndex &parent);
    QHash<int, QByteArray> roleNames() const;

    // OrnAbstractListModel interface
protected:
    void processReply(const QJsonDocument &jsonDoc);
};

#endif // ORNCATEGORIESMODEL_H
