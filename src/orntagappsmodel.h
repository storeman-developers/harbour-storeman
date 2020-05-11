#ifndef ORNTAGAPPSMODEL_H
#define ORNTAGAPPSMODEL_H


#include "ornabstractappsmodel.h"

class OrnTagAppsModel : public OrnAbstractAppsModel
{
    Q_OBJECT
    Q_PROPERTY(quint32 tagId READ tagId WRITE setTagId NOTIFY tagIdChanged)

public:
    OrnTagAppsModel(QObject *parent = nullptr);

    quint32 tagId() const;
    void setTagId(quint32 tagId);

signals:
    void tagIdChanged();

private:
    quint32 mTagId{0};

    // QAbstractItemModel interface
public:
    void fetchMore(const QModelIndex &parent) override;
};

#endif // ORNTAGAPPSMODEL_H
