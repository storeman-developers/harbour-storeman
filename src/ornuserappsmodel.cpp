#include "ornuserappsmodel.h"

OrnUserAppsModel::OrnUserAppsModel(QObject *parent) :
    OrnAbstractAppsModel(false, parent),
    mUserId(0)
{

}

quint32 OrnUserAppsModel::userId() const
{
    return mUserId;
}

void OrnUserAppsModel::setUserId(const quint32 &userId)
{
    if (mUserId != userId)
    {
        mUserId = userId;
        emit this->userIdChanged();
        this->reset();
    }
}

void OrnUserAppsModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
    {
        return;
    }
    OrnAbstractListModel::apiCall(QStringLiteral("users/%0/apps").arg(mUserId));
}
