#include "orntagappsmodel.h"

OrnTagAppsModel::OrnTagAppsModel(QObject *parent)
    : OrnAbstractAppsModel(true, parent)
    , mTagId(0)
{}

quint32 OrnTagAppsModel::tagId() const
{
    return mTagId;
}

void OrnTagAppsModel::setTagId(quint32 tagId)
{
    if (mTagId != tagId)
    {
        mTagId = tagId;
        emit this->tagIdChanged();
        this->reset();
    }
}

void OrnTagAppsModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid() || mTagId == 0)
    {
        return;
    }

    OrnAbstractListModel::fetch(QStringLiteral("tags/%1/apps").arg(mTagId));
}
