#ifndef ORN_TAGSMODEL_H
#define ORN_TAGSMODEL_H

#include "ornabstractlistmodel.h"
#include "orntaglistitem.h"

class OrnTagsModel : public OrnAbstractListModel<OrnTagListItem>
{
    Q_OBJECT
    Q_PROPERTY(QStringList tagIds READ tagIds WRITE setTagIds NOTIFY tagIdsChanged)

public:
    enum Role
    {
        TagIdRole = Qt::UserRole,
        AppsCountRole,
        NameRole
    };
    Q_ENUM(Role)

    explicit OrnTagsModel(QObject *parent = nullptr);

    QStringList tagIds() const;
    void setTagIds(const QStringList &ids);

signals:
    void tagIdsChanged();

private:
    QStringList mTagIds;
    QJsonArray  mFetchedTags;

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    void fetchMore(const QModelIndex &parent);
};

#endif // ORN_TAGSMODEL_H
