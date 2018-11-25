#include "ornrecentappsmodel.h"

OrnRecentAppsModel::OrnRecentAppsModel(QObject *parent) :
    OrnAbstractAppsModel(true, parent)
{

}

void OrnRecentAppsModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
    {
        return;
    }
    OrnAbstractListModel::fetch(QStringLiteral("apps"));
}
